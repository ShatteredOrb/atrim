#include "impl/OcclusionQueryOGL.h"
#include "impl/LowLevelGraphicsSDL.h"

#include <GL/glew.h>


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cOcclusionQueryOGL::cOcclusionQueryOGL()
{
    glGenQueriesARB(1, (GLuint *)&mlQueryId);
    mlLastSampleCount = 0;
    mbResultsAreFetched = true;
}

//-----------------------------------------------------------------------

cOcclusionQueryOGL::~cOcclusionQueryOGL()
{
    glDeleteQueriesARB(1, (GLuint *)&mlQueryId);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cOcclusionQueryOGL::Begin()
{
    glBeginQueryARB(GL_SAMPLES_PASSED_ARB, mlQueryId);
    mbResultsAreFetched = false;
}

void cOcclusionQueryOGL::End()
{
    glEndQueryARB(GL_SAMPLES_PASSED_ARB);
}

bool cOcclusionQueryOGL::FetchResults()
{
    if(mbResultsAreFetched)
    {
        return true;
    }

    //if it is not a query, there are no samples to get!
    if(glIsQuery(mlQueryId) == GL_FALSE)
    {
        mbResultsAreFetched = true;
        mlLastSampleCount = 0;
        return true;
    }

    GLint lAvailable = 0;
    glGetQueryObjectivARB(mlQueryId, GL_QUERY_RESULT_AVAILABLE_ARB, &lAvailable);
    if(lAvailable == 0)
    {
        return false;
    }

    glGetQueryObjectivARB(mlQueryId, GL_QUERY_RESULT_ARB, (GLint *)&mlLastSampleCount);

    mbResultsAreFetched = true;

    return true;
}

void cOcclusionQueryOGL::WaitForResults()
{
    if(mbResultsAreFetched)
    {
        return;
    }

    //if it is not a query, there are no samples to get!
    if(glIsQuery(mlQueryId) == GL_FALSE)
    {
        mbResultsAreFetched = true;
        mlLastSampleCount = 0;
        return;
    }

    //GL_QUERY_RESULT blocks inside the driver until the result is available.
    glGetQueryObjectivARB(mlQueryId, GL_QUERY_RESULT_ARB, (GLint *)&mlLastSampleCount);

    mbResultsAreFetched = true;
}

//-----------------------------------------------------------------------

unsigned int cOcclusionQueryOGL::GetSampleCount()
{
    return mlLastSampleCount;
}

//-----------------------------------------------------------------------
}
