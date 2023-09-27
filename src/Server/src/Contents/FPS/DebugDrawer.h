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
        // 여기서 선을 그립니다. 예를 들어, OpenGL을 사용할 경우
        glBegin(GL_LINES);
        glColor3f(color.x(), color.y(), color.z());
        glVertex3f(from.x(), from.y(), from.z());
        glVertex3f(to.x(), to.y(), to.z());
        glEnd();
    }

    virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        // 접점과 노멀을 그릴 수 있습니다. 여기에서는 단순히 접점만 그립니다.
        glBegin(GL_POINTS);
        glColor3f(color.x(), color.y(), color.z());
        glVertex3f(pointOnB.x(), pointOnB.y(), pointOnB.z());
        glEnd();
    }

    virtual void draw3dText(const btVector3& location, const char* textString)
    {
        // 3D 텍스트를 그릴 수 있는 방법이 필요합니다.
        // 이 예제에서는 이 기능을 구현하지 않았지만, 특수한 렌더링 라이브러리를 사용해야 합니다.
        // 예를 들어, OpenGL과 FTGL 라이브러리를 사용하여 3D 텍스트를 그릴 수 있습니다.
    }

    virtual void reportErrorWarning(const char* warningString)
    {
        // 경고 메시지를 출력합니다.
        std::cerr << warningString << std::endl;
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