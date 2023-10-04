#pragma once

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
};