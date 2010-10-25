/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of the class Texture.
 *
 *   @author Lionel Heng <hengli@student.ethz.ch>
 *
 */

#include "Texture.h"

Texture::Texture()
{

}

const QString&
Texture::getSourceURL(void) const
{
    return sourceURL;
}

void
Texture::setID(GLuint id)
{
    this->id = id;
}

void
Texture::sync(const WebImagePtr& image)
{
    state = static_cast<State>(image->getState());

    if (image->getState() != WebImage::UNINITIALIZED &&
        sourceURL != image->getSourceURL())
    {
        sourceURL = image->getSourceURL();
    }

    if (image->getState() == WebImage::READY && image->getSyncFlag())
    {
        image->setSyncFlag(false);

        if (image->getWidth() != imageWidth ||
            image->getHeight() != imageHeight)
        {
            imageWidth = image->getWidth();
            textureWidth = 32;
            while (textureWidth < imageWidth)
            {
                textureWidth *= 2;
            }
            imageHeight = image->getHeight();
            textureHeight = 32;
            while (textureHeight < imageHeight)
            {
                textureHeight *= 2;
            }

            maxU = static_cast<double>(imageWidth)
                   / static_cast<double>(textureWidth);
            maxV = static_cast<double>(imageHeight)
                   / static_cast<double>(textureHeight);

            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, textureWidth, textureHeight,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        }

        glBindTexture(GL_TEXTURE_2D, id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight,
                        GL_RGBA, GL_UNSIGNED_BYTE, image->getData());
    }
}

void
Texture::draw(float x1, float y1, float x2, float y2,
              bool smoothInterpolation) const
{
    draw(x1, y1, x2, y1, x2, y2, x1, y2, smoothInterpolation);
}

void
Texture::draw(float x1, float y1, float x2, float y2,
              float x3, float y3, float x4, float y4,
              bool smoothInterpolation) const
{
    if (state == REQUESTED)
    {
        glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
        glVertex2f(x4, y4);
        glEnd();

        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id);

    float dx, dy;
    if (smoothInterpolation)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        dx = 1.0f / (2.0f * textureWidth);
        dy = 1.0f / (2.0f * textureHeight);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        dx = 0.0f;
        dy = 0.0f;
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);

    glTexCoord2f(dx, maxV - dy);
    glVertex2f(x1, y1);
    glTexCoord2f(maxU - dx, maxV - dy);
    glVertex2f(x2, y2);
    glTexCoord2f(maxU - dx, dy);
    glVertex2f(x3, y3);
    glTexCoord2f(dx, dy);
    glVertex2f(x4, y4);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}
