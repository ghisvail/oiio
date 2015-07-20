/* ====================================================================
 * Copyright (c) 2015, Imperial College London
 * Copyright (c) 2015, Ghislain Antony Vaillant
 * All rights reserved.
 *
 * Distributed under the terms of the new BSD license.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * ====================================================================
 * */

#include "gipl_pvt.h"

OIIO_PLUGIN_NAMESPACE_BEGIN

// Obligatory material to make this a recognizeable imageio plugin:
OIIO_PLUGIN_EXPORTS_BEGIN

OIIO_EXPORT int gipl_imageio_version = OIIO_PLUGIN_VERSION;

OIIO_EXPORT ImageInput *gipl_input_imageio_create () {
  return new GiplInput;
}

OIIO_EXPORT const char *gipl_input_extensions[] = {
  "gipl", NULL
};

OIIO_PLUGIN_EXPORTS_END

bool
GiplInput::valid_file (const std::string &filename) const
{
  FILE *fd = Filesystem::fopen (filename, "rb");
  if (! fd) return false;
  uint32_t magic;
  bool ok = (std::fread (&magic, sizeof (magic), 1, fd) == 1) &&
      ((magic == gipl_pvt::GIPL_MAGIC_NUMBER1) ||
       (magic == gipl_pvt::GIPL_MAGIC_NUMBER2) );
  fclose (fd);
  return ok;
}

bool
GiplInput::open (const std::string &name, ImageSpec &spec)
{
  m_filename = name;

  m_fd = Filesystem::fopen(m_filename, "rb");
  if (!m_fd)
  {
    error ("Could not open file \"%s\"", name.c_str());
    return false;
  }

  if (!read_header()) // will also take care of potential read errors
    return false;

  if ((m_header.magic_number == gipl_pvt::GIPL_MAGIC_NUMBER1) ||
      (m_header.magic_number == gipl_pvt::GIPL_MAGIC_NUMBER2))
  {
    error("\"%s\" is not a %s file, magic number doesn't match",
        m_filename.c_str(), format_name());
    close();
    return false;
  }

  m_spec = ImageSpec();
  m_spec.width = m_header.dim[0];
  m_spec.height = m_header.dim[1];
  m_spec.depth = m_header.dim[2];
  m_spec.attribute("XResolution", m_header.pixdim[0]);
  m_spec.attribute("YResolution", m_header.pixdim[1]);
  m_spec.attribute("ResolutionUnit", "mm");

  switch(m_header.image_type)
  {
    case gipl_pvt::IT_CHAR:
      m_spec.set_format(TypeDesc::INT8);
      break;
    case gipl_pvt::IT_U_CHAR:
      m_spec.set_format(TypeDesc::UINT8);
      break;
    case gipl_pvt::IT_SHORT:
      m_spec.set_format(TypeDesc::INT16);
      break;
    case gipl_pvt::IT_U_SHORT:
      m_spec.set_format(TypeDesc::UINT16);
      break;
    case gipl_pvt::IT_U_INT:
      m_spec.set_format(TypeDesc::UINT);
      break;
    case gipl_pvt::IT_INT:
      m_spec.set_format(TypeDesc::INT);
      break;
    case gipl_pvt::IT_FLOAT:
      m_spec.set_format(TypeDesc::FLOAT);
      break;
    case gipl_pvt::IT_DOUBLE:
      m_spec.set_format(TypeDesc::DOUBLE);
      break;
/*
    case gipl_pvt::IT_VECTOR_3D_CHAR:
      m_spec.set_format(TypeDesc(TypeDesc::INT8, TypeDesc::VEC3));
      break;
    case gipl_pvt::IT_VECTOR_3D_SHORT:
      m_spec.set_format(TypeDesc(TypeDesc::INT16, TypeDesc::VEC3));
      break;
    case gipl_pvt::IT_VECTOR_3D_FLOAT:
      m_spec.set_format(TypeDesc(TypeDesc::FLOAT, TypeDesc::VEC3));
      break;
    case gipl_pvt::IT_VECTOR_3D_DOUBLE:
      m_spec.set_format(TypeDesc(TypeDesc::DOUBLE, TypeDesc::VEC3));
      break;
    case gipl_pvt::IT_C_SHORT:
      m_spec.set_format(TypeDesc(TypeDesc::INT16, TypeDesc::VEC2));
      break;
    case gipl_pvt::IT_C_INT:
      m_spec.set_format(TypeDesc(TypeDesc::INT, TypeDesc::VEC2));
      break;
    case gipl_pvt::IT_C_FLOAT:
      m_spec.set_format(TypeDesc(TypeDesc::FLOAT, TypeDesc::VEC2));
      break;
    case gipl_pvt::IT_C_DOUBLE:
      m_spec.set_format(TypeDesc(TypeDesc::DOUBLE, TypeDesc::VEC2));
      break;
*/
    default:
      error("Image type not supported");
      close();
      return false;
  }

  switch(m_header.flag1)
  {
    case gipl_pvt::UNDEFINED:
      m_spec.attribute("gipl:orientation", "undefined");
      break;
    case gipl_pvt::UNDEFINED_PROJECTION:
      m_spec.attribute("gipl:orientation", "undefined_projection");
      break;
    case gipl_pvt::AP_PROJECTION:
      m_spec.attribute("gipl:orientation", "ap_projection");
      break;
    case gipl_pvt::LATERAL_PROJECTION:
      m_spec.attribute("gipl:orientation", "lateral_projection");
      break;
    case gipl_pvt::OBLIQUE_PROJECTION:
      m_spec.attribute("gipl:orientation", "oblique_projection");
      break;
    case gipl_pvt::UNDEFINED_TOMO:
      m_spec.attribute("gipl:orientation", "undefined_tomo");
      break;
    case gipl_pvt::AXIAL:
      m_spec.attribute("gipl:orientation", "axial");
      break;
    case gipl_pvt::CORONAL:
      m_spec.attribute("gipl:orientation", "coronal");
      break;
    case gipl_pvt::SAGITTAL:
      m_spec.attribute("gipl:orientation", "sagittal");
      break;
    case gipl_pvt::OBLIQUE_TOMO:
      m_spec.attribute("gipl:orientation", "oblique_tomo");
      break;
  }

  if (strlen(m_header.line1))
    m_spec.attribute("ImageDescription", m_header.line1);

  return true;
}

bool
GiplInput::read_native_scanline(int y, int z, void *data)
{
  std::vector<unsigned char> scanline_data(m_spec.scanline_bytes());
  long scanline_offset = (m_spec.depth - z) * m_spec.tile_bytes() +
                         (m_spec.height - y) * m_spec.scanline_bytes();

  // position file pointer to beginning of scanline
  fseek(m_fd, scanline_offset, SEEK_CUR);

  // read content of scanline
  fread(&scanline_data[0], sizeof(unsigned char), scanline_data.size());
  
  // GIPL format is big-endian, swap if required
  if (littleendian ())
  {
    if(m_spec.format == TypeDesc::UINT8)
      swap_endian((uint8_t*)&scanline_data[0],
          scanline_data.size() / sizeof(uint8_t));

    else if(m_spec.format == TypeDesc::INT8)
      swap_endian((int8_t*)&scanline_data[0],
          scanline_data.size() / sizeof(int8_t));

    else if(m_spec.format == TypeDesc::UINT16)
      swap_endian((uint16_t*)&scanline_data[0],
          scanline_data.size() / sizeof(uint16_t));

    else if(m_spec.format == TypeDesc::INT16)
      swap_endian((int16_t*)&scanline_data[0],
          scanline_data.size() / sizeof(int16_t));

    else if(m_spec.format == TypeDesc::UINT)
      swap_endian((uint32_t*)&scanline_data[0],
          scanline_data.size() / sizeof(uint32_t));

    else if(m_spec.format == TypeDesc::INT)
      swap_endian((int32_t*)&scanline_data[0],
          scanline_data.size() / sizeof(uint32_t));

    else if(m_spec.format == TypeDesc::FLOAT)
      swap_endian((float*)&scanline_data[0],
          scanline_data.size() / sizeof(float));

    else if(m_spec.format == TypeDesc::DOUBLE)
      swap_endian((double*)&scanline_data[0],
          scanline_data.size() / sizeof(double));
  }

  // copy scanline content to output buffer
  memcpy(data, &scanline_data[0], scanline_data.size());
  
  // reset file pointer to beginning of data
  fseek(m_fd, gipl_pvt::GIPL_HEADER_SIZE, SEEK_SET);
  
  return true;
}

void
GiplInput::init () {
  m_fd = NULL;
  m_filename.clear();
}

bool
GiplInput::read_header()
{
  if( !fread(&m_header.dim)           ||
      !fread(&m_header.image_type)    ||
      !fread(&m_header.pixdim)        ||
      !fread(&m_header.line1)         ||
      !fread(&m_header.matrix)        ||
      !fread(&m_header.flag1)         ||
      !fread(&m_header.flag2)         ||
      !fread(&m_header.min)           ||
      !fread(&m_header.max)           ||
      !fread(&m_header.origin)        ||
      !fread(&m_header.pixval_offset) ||
      !fread(&m_header.pixval_cal)    ||
      !fread(&m_header.user_def1)     ||
      !fread(&m_header.user_def2)     ||
      !fread(&m_header.magic_number)  )
  {
    error ("Could not read header of file \"%s\"", m_filename.c_str());
    return false;
  }

  // ensure line1 attribute is a well-formed c-string 
  m_header.line1[79] = '\0';

  // GIPL format is big-endian, swap if required
  if(littleendian())
  {
    swap_endian(&m_header.dim, 4);
    swap_endian(&m_header.image_type);
    swap_endian(&m_header.pixdim, 4);
    swap_endian(&m_header.line1, 80);
    swap_endian(&m_header.matrix, 20);
    swap_endian(&m_header.flag1);
    swap_endian(&m_header.flag2);
    swap_endian(&m_header.min);
    swap_endian(&m_header.max);
    swap_endian(&m_header.origin, 4);
    swap_endian(&m_header.pixval_offset);
    swap_endian(&m_header.pixval_cal);
    swap_endian(&m_header.user_def1);
    swap_endian(&m_header.user_def2);
    swap_endian(&m_header.magic_number);
  }

  return true;
}

OIIO_PLUGIN_NAMESPACE_END
