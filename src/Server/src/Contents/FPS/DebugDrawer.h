#pragma once

#if _WIN32
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glu.h>

#include <btBulletDynamicsCommon.h>

class BulletDebugDrawer : public btIDebugDraw
{
    int m_debugMode;

public:
    BulletDebugDrawer() : m_debugMode(0) {}

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        glBegin(GL_LINES);
        glColor3f(color.x(), color.y(), color.z());
        glVertex3f(from.x(), from.y(), from.z());
        glVertex3f(to.x(), to.y(), to.z());
        glEnd();
    }

    virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        glBegin(GL_POINTS);
        glColor3f(color.x(), color.y(), color.z());
        glVertex3f(pointOnB.x(), pointOnB.y(), pointOnB.z());
        glEnd();
    }

    virtual void draw3dText(const btVector3& location, const char* textString)
    {
    }

    virtual void reportErrorWarning(const char* warningString)
    {
    }

    virtual void setDebugMode(int debugMode)
    {
        m_debugMode = debugMode;
    }

    virtual int getDebugMode() const
    {
        return m_debugMode;
    }

    virtual void drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar alpha) override
    {
        glBegin(GL_TRIANGLES);
		glColor4f(color.x(), color.y(), color.z(), alpha);
		glVertex3f(v0.x(), v0.y(), v0.z());
		glVertex3f(v1.x(), v1.y(), v1.z());
		glVertex3f(v2.x(), v2.y(), v2.z());
		glEnd();
    }

    void drawBox(const btVector3& bbMin, const btVector3& bbMax, const btVector3& color) override
    {
        drawTriangle(bbMin, btVector3(bbMax[0], bbMin[1], bbMin[2]), btVector3(bbMax[0], bbMax[1], bbMin[2]), color, 1.0f);
        drawTriangle(bbMin, btVector3(bbMax[0], bbMax[1], bbMin[2]), btVector3(bbMin[0], bbMax[1], bbMin[2]), color, 1.0f);

        drawLine(btVector3(bbMin[0], bbMin[1], bbMin[2]), btVector3(bbMax[0], bbMin[1], bbMin[2]), color);
        drawLine(btVector3(bbMax[0], bbMin[1], bbMin[2]), btVector3(bbMax[0], bbMax[1], bbMin[2]), color);
        drawLine(btVector3(bbMax[0], bbMax[1], bbMin[2]), btVector3(bbMin[0], bbMax[1], bbMin[2]), color);
        drawLine(btVector3(bbMin[0], bbMax[1], bbMin[2]), btVector3(bbMin[0], bbMin[1], bbMin[2]), color);
        drawLine(btVector3(bbMin[0], bbMin[1], bbMin[2]), btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMin[1], bbMin[2]), btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMax[1], bbMin[2]), btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMin[0], bbMax[1], bbMin[2]), btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMin[0], bbMin[1], bbMax[2]), btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMin[1], bbMax[2]), btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMax[1], bbMax[2]), btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMin[0], bbMax[1], bbMax[2]), btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
    }

    virtual void drawBox(const btVector3& bbMin, const btVector3& bbMax, const btTransform& trans, const btVector3& color) override
    {
        btVector3 vertices[8] = {
        {bbMin[0], bbMin[1], bbMin[2]},
        {bbMax[0], bbMin[1], bbMin[2]},
        {bbMax[0], bbMax[1], bbMin[2]},
        {bbMin[0], bbMax[1], bbMin[2]},
        {bbMin[0], bbMin[1], bbMax[2]},
        {bbMax[0], bbMin[1], bbMax[2]},
        {bbMax[0], bbMax[1], bbMax[2]},
        {bbMin[0], bbMax[1], bbMax[2]}
        };

        // Define indices for the triangles of each face of the box


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        int indices[12][3] = {
            {0, 1, 2}, {2, 3, 0}, // front face
            {4, 5, 6}, {6, 7, 4}, // back face
            {0, 1, 5}, {5, 4, 0}, // bottom face
            {2, 3, 7}, {7, 6, 2}, // top face
            {0, 4, 7}, {7, 3, 0}, // left face
            {1, 5, 6}, {6, 2, 1}  // right face
        };

        for (int i = 0; i < 12; i++) {
            drawTriangle(
                trans * vertices[indices[i][0]],
                trans * vertices[indices[i][1]],
                trans * vertices[indices[i][2]],
                color, 0.5f);
        }

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
    }
};
#endif