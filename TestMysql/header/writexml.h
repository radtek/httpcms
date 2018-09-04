#pragma once
#include "usMysql.h"

/*从path路径下的xml中读取数据，并作修改后，写在destpath的xml中，后两个参数是为了方便添加的*/
int writexml(std::string path, std::string destpath, std::string datedate, std::string producer);
