//
//  Menu.hpp
//  103033241_AS4
//
//  Created by Softcat on 1/8/19.
//  Copyright © 2019 NTHU. All rights reserved.
//

#ifndef Menu_hpp
#define Menu_hpp

#include <stdio.h>
#include <iostream>

class Menu{
public:
    Menu();
    int now();
    int _now;
    const int _MENU_TIMER_START = 1;
    const int _MENU_TIMER_STOP = 2;
    const int _MENU_EXIT = 3;
    const int _DEPTH_LIGHT = 4;
    const int _DEPTH_EYE = 5;
    const int _SSAO = 6;
    const int _EYE = 7;
};

#endif /* Menu_hpp */
