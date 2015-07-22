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

#include <cstdio>

#include "OpenImageIO/filesystem.h"
#include "OpenImageIO/fmath.h"
#include "OpenImageIO/imageio.h"

#include "gipl_pvt.h"
using namespace OIIO_NAMESPACE::gipl_pvt;


OIIO_PLUGIN_NAMESPACE_BEGIN


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

    // helper function for safer file reading
    template <class T>
    bool fwrite(const T *buffer, std::size_t size=sizeof(T),
                std::size_t count=1)
    {
      std::size_t nitems = std::fwrite((const void *)buffer, size, count, m_fd);
      if(nitems != count)
        error("Error while writing to file \"%s\" (wrote %d of %d records)",
            m_filename, (int)nitems , (int)count);
      return nitems == count;
    }
};


// Obligatory material to make this a recognizeable imageio plugin
OIIO_PLUGIN_EXPORTS_BEGIN

OIIO_EXPORT ImageOutput *
gipl_output_imageio_create()
{
  return new GiplOutput;
}

OIIO_EXPORT const char * gipl_output_extensions[] = { "gipl", NULL };

OIIO_PLUGIN_EXPORTS_END

void
GiplOutput::init () {
  m_fd = NULL;
  m_filename.clear();
}

OIIO_PLUGIN_NAMESPACE_END
