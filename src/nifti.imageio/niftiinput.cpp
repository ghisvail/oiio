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

class NiftiInput: public ImageInput
{
  public:
    NiftiInput() { init(); }
    virtual ~NiftiInput() { close(); }
    virtual const char *format_name(void) const { return "nifti"; }
    virtual bool valid_file(const std::string &filename) const;
    virtual bool open(const std::string &name, ImageSpec &spec);
    virtual bool close(void);
    virtual bool read_native_scanline(int y, int z, void *data);
  private:
    void init();
};

// Obligatory material to make this a recognizeable imageio plugin:
OIIO_PLUGIN_EXPORTS_BEGIN

  OIIO_EXPORT int nifti_imageio_version = OIIO_PLUGIN_VERSION;

  OIIO_EXPORT ImageInput *nifti_input_imageio_create()
  {
    return new NiftiInput;
  }

  OIIO_EXPORT const char *nifti_input_extensions[] = { "nii", "nifti", NULL };

OIIO_PLUGIN_EXPORTS_END

OIIO_PLUGIN_NAMESPACE_END
