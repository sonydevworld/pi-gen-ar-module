#pragma once

/**
 *      Initiate the webrequest api. This function will register it self in the fwatch module.
 */
int webrequest_init();

/**
 *
 *      Will close and clean used resources.
 */
void webrequest_close();
