/* ------------------------------------------------------------------
global.h is part of picture-show2.
.....................................................................

picture-show is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

picture-show2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with picture-show. If not, see <http://www.gnu.org/licenses/>.

......................................................................

author: Sebastian Schäfer
February 2013

--------------------------------------------------------------------*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

enum ScaleType {
    IMAGE,
    SCREEN
};

enum BlendType {
    FADE,
    FADE_BLACK,
    SLIDE,
    TWIST_FADE,
    ANDROID_STYLE,
    HARD
};

enum Sorting {
    FILENAME,
    DATE_CREATED,
    RANDOM
};

enum ImageState {
    IMAGE_LOADING,
    IMAGE_READY,
    IMAGE_ERROR
};

enum LoadingType {
    MEMORY_OPTIMIZED,
    PERFORMANCE_OPTIMIZED
};

enum Language {
    GERMAN,
    ENGLISH
};


#endif // GLOBAL_H
