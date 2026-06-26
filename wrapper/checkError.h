//
//  checkError.h
//  TryGL
//
//  Created by 邓和平 on 2025/8/19.
//

#pragma once

#ifdef DEBUG
#define GL_CALL(func) func;checkError();
#else
#define GL_CALL(func) func;
#endif

void checkError();
