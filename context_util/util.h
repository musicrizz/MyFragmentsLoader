//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * util.h
 *
 *  Created on: Dec 5, 2020
 *      Author: gio
 */
#pragma once
#ifndef OPENGL_CONTEXT_UTIL_UTIL_H_
#define OPENGL_CONTEXT_UTIL_UTIL_H_

#include "CommonHeaders.h"
#include "ShaderMap.h"
#include "OpenGLContext.h"
#include "OpenGLerror.h"
#include "TempoMap.h"

inline int mipmapsLevels (int w, int h, int stop = 4)   {
    int size = fmin(w,h);
    int num = 1;
    while(true)  {
        size /= 2;
        if(size < stop) break;
        num++;
    }
    return num;
}

#endif /* OPENGL_CONTEXT_UTIL_UTIL_H_ */
