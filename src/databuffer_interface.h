#ifndef __DATABUFFER_INTERFACE_H
#define __DATABUFFER_INTERFACE_H

class databufferInterface
{
public:
    databufferInterface() {}
    virtual ~databufferInterface() {}


private:
    databufferInterface( const databufferInterface& source );
    void operator = ( const databufferInterface& source );
};


#endif // __DATABUFFER_INTERFACE_H
