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

#ifndef OPENIMAGEIO_GIPL_H
#define OPENIMAGEIO_GIPL_H

#include <cstdio>
#include "OpenImageIO/imageio.h"
#include "OpenImageIO/filesystem.h"
#include "OpenImageIO/fmath.h"

OIIO_PLUGIN_NAMESPACE_BEGIN

namespace gipl_pvt {

// GIPL magic number
static const uint32_t GIPL_MAGIC_NUMBER1 = 0xefffe9b0;
static const uint32_t GIPL_MAGIC_NUMBER2 = 0x2ae389b8;

// GIPL header size
static const size_t GIPL_HEADER_SIZE = 256;

// GIPL image types
enum ImageTypes {
  IT_BINARY = 1,
  IT_CHAR = 7,
  IT_U_CHAR = 8,
  IT_SHORT = 15,
  IT_U_SHORT = 16,
  IT_U_INT = 31,
  IT_INT = 32,
  IT_FLOAT = 64,
  IT_DOUBLE = 65,
  IT_VECTOR_3D_CHAR = 66,
  IT_VECTOR_3D_SHORT = 67,
  IT_VECTOR_3D_FLOAT = 68,
  IT_VECTOR_3D_DOUBLE = 69,
  IT_C_SHORT = 144,
  IT_C_INT = 160,
  IT_C_FLOAT = 192,
  IT_C_DOUBLE = 193,
  IT_SURFACE = 200,
  IT_POLYGON = 201,
};

// GIPL orientations
enum Orientations {
  UNDEFINED = 0,
  UNDEFINED_PROJECTION = 1,
  AP_PROJECTION = 2,
  LATERAL_PROJECTION = 3,
  OBLIQUE_PROJECTION = 4,
  UNDEFINED_TOMO = 8,
  AXIAL = 9,
  CORONAL = 10,
  SAGITTAL = 11,
  OBLIQUE_TOMO = 12,
};

// GIPL header layout
struct GiplHeader         /* Offset | Size | Description                    */
{                         /* -----------------------------------------------*/
  uint16_t dim[4];        /*      0 |    8 | X,Y,Z,T image dimensions       */
  uint16_t image_type;    /*      8 |    2 | Type of image data             */
  float pixdim[4];        /*     10 |   16 | X,Y,Z,T pixel dimensions mm    */
  char line1[80];         /*     26 |   80 | Patient / text field           */
  float matrix[20];       /*    106 |   20 | Transformation matrix          */
  uint8_t flag1;          /*    186 |    1 | Orientation flag               */
  uint8_t flag2;          /*    187 |    1 | Unknown                        */
  double min;             /*    188 |    8 | Minimum voxel value            */
  double max;             /*    196 |    8 | Maximum voxel value            */
  double origin[4];       /*    204 |   32 | X,Y,Z,T offset                 */
  float pixval_offset;    /*    236 |    4 | Unknown                        */
  float pixval_cal;       /*    240 |    4 | Unknown                        */
  float user_def1;        /*    244 |    4 | Inter-slice gap                */
  float user_def2;        /*    248 |    4 | User defined field             */
  uint32_t magic_number;  /*    252 |    4 | For verification purposes      */
                          /* Total = 256 bytes                              */
};

}; // namespace gipl_pvt

class GiplInput: public ImageInput {
  public:
    GiplInput () { init (); }
    virtual ~GiplInput () { close (); }
    virtual const char *format_name (void) const { return "gipl"; }
    virtual bool valid_file (const std::string &filename) const;
    virtual bool open (const std::string &name, ImageSpec &spec);
    virtual bool close (void);
    virtual bool read_native_scanline (int y, int z, void *data);
  private:
    FILE *m_fd;
    std::string m_filename;
    gipl_pvt::GiplHeader m_header;
    void init ();
    bool read_header();

    // helper function for safe file reading
    template <class T>
    bool fread(const T *buffer, std::size_t size=sizeof(T),
               std::size_t count=1)
    {
      size_t nread = std::fwrite(buffer, size, count, m_fd);
      if(nread != count)
        error("Error reading file \"%s\" (wrote %d of %d records)",
            m_filename, (int)nread , (int)count);
      return nread == count;
    }
};

class GiplOutput: public ImageOutput {
  public:
    GiplOutput () { init (); }
    virtual ~GiplOutput () { close (); }
    virtual const char *format_name (void) const { return "gipl"; }
    virtual int supports (string_view feature) const;
    virtual bool open (const std::string &name, const ImageSpec &spec,
                       OpenMode mode=Create);
    virtual bool close (void);
    virtual bool write_scanline (int y, int z, TypeDesc format,
                                 const void *data, stride_t xstride);
    virtual bool write_tile (int x, int y, int z, TypeDesc format,
                             const void *data, stride_t xstride,
                             stride_t ystride, stride_t zstride);
  private:
    FILE *m_fd;
    std::string m_filename;
    void init ();
};

OIIO_PLUGIN_NAMESPACE_END

#endif // OPENIMAGEIO_GIPL_H
