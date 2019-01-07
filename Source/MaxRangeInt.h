//
//  MaxRangeInt.h
//  103033241_AS4
//
//  Created by Softcat on 12/28/18.
//  Copyright © 2018 NTHU. All rights reserved.
//
/*
 * An class to indicate which texture should be draw to screen.
 */

#ifndef MaxRangeInt_h
#define MaxRangeInt_h

class MaxRangeInt{
public:
    MaxRangeInt(int max){_max = max; _val = 0;};
    void add(int val){_val = (_val+val)%_max;};
    int get(){return _val;};
private:
    int _max;
    int _val;
};

#endif /* MaxRangeInt_h */
