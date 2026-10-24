#ifndef HPL_OCCLUSION_QUERY_H
#define HPL_OCCLUSION_QUERY_H

namespace hpl
{

class iOcclusionQuery
{
public:
    virtual ~iOcclusionQuery() {}

    /**
     * Starts the query. Must be followed by an End().
     */
    virtual void Begin() = 0;
    /**
     * Ends the current query. Must have been started with a Begin()
     */
    virtual void End() = 0;

    /**
     * Fetches the result of he query. Should not be done until all is rendered.
     */
    virtual bool FetchResults() = 0;

    /**
     * Waits until the result is in and fetches it. Lets the driver do the waiting instead of
     * spinning on FetchResults() (every spin is a round trip to the driver, which is extra
     * slow through layers like Wine).
     */
    virtual void WaitForResults()
    {
        while(FetchResults() == false);
    }

    /**
     * Get the number of samples that passed stencil and depth test in the query begin/end.
     */
    virtual unsigned int GetSampleCount() = 0;
};

};
#endif // HPL_OCCLUSION_QUERY_H
