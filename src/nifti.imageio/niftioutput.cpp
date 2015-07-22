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

#include "OpenImageIO/imageio.h"

OIIO_PLUGIN_NAMESPACE_BEGIN

class NiftiOutput: public ImageOutput
{
  public:
    NiftiOutput() { init(); }
    virtual ~NiftiOutput() { close(); }
    virtual const char *format_name(void) const { return "nifti"; }
    virtual int supports(string_view feature) const;
    virtual bool open(const std::string &name, const ImageSpec &spec,
                      OpenMode mode=Create);
    virtual bool close(void);
    virtual bool write_scanline(int y, int z, TypeDesc format,
                                const void *data, stride_t xstride);
  private:
    void init();
};

// Obligatory material to make this a recognizeable imageio plugin
OIIO_PLUGIN_EXPORTS_BEGIN

  OIIO_EXPORT ImageOutput *nifti_output_imageio_create()
  {
    return new NiftiOutput;
  }

  OIIO_EXPORT const char *nifti_output_extensions[] = { "nii", "nifti", NULL };

OIIO_PLUGIN_EXPORTS_END

OIIO_PLUGIN_NAMESPACE_END
