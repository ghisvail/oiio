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

OIIO_PLUGIN_NAMESPACE_END
