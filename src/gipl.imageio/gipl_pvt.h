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

#include "OpenImageIO/imageio.h"

OIIO_PLUGIN_NAMESPACE_BEGIN

namespace gipl_pvt {


}; // namespace gipl_pvt

class GiplInput: public ImageInput {
  public:
    GiplInput () { /* */ }
    virtual ~GiplInput () { close(); }
    virtual const char *format_name (void) const { return "gipl"; }
    virtual bool valid_file (const std::string &filename) const;
    virtual bool open (const std::string &name, ImageSpec &spec);
    virtual bool close (void);
    virtual bool read_native_scanline (int y, int z, void *data);
  private:
    FILE *m_fd;
    std::string m_filename;
};

class GiplOutput: public ImageOutput {
  public:
    GiplOutput () : m_fd(NULL) { }
    virtual ~GiplOutput () { close(); }
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
};

OIIO_PLUGIN_NAMESPACE_END

#endif // OPENIMAGEIO_GIPL_H
