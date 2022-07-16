#include "utils.h"

#include "constants.h"

#include <GL/freeglut.h>
#include <GL/gl.h>

namespace rendering
{
void renderText(const std::string& s, double x, double y)
{
    glPushMatrix();
    glTranslated(x, y, 0);
    glScalef(0.1, 0.1, 1);

    glColor3fv(constants::colors::TEXT.data());
    for (const auto c : s)
    {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c);
    }

    glPopMatrix();
}
}
