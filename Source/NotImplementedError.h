//
//  NotImplementedError.h
//  103033241_AS4
//
//  Created by Softcat on 12/18/18.
//  Copyright © 2018 NTHU. All rights reserved(?).
//  from:
//      https://stackoverflow.com/questions/24469927/does-c-have-an-equivalent-to-nets-notimplementedexception

#ifndef NotImplementedError_h
#define NotImplementedError_h

class NotImplemented : public std::logic_error
{
public:
    NotImplemented() : std::logic_error("Function not yet implemented") { };
};

#endif /* NotImplementedError_h */
