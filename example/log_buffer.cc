#include"log_buffer.hpp"
#include<stdio.h>
#include<string.h>

int main(){
	FILE* fp=NULL;
	fp=fopen("../tmp/log_buffer.txt","w");
    printf("headnodesize%d\n",headnode_size);///268
	Buffer buff(100000000);

	const char* str="buffer is ok...\n";
	for(int i=0;i<1000000;i++){
		buff.append(str,strlen(str));
	}

	buff.try_write(fp);
	fclose(fp);

	printf("可用长度:%d\n",buff.available_len());
	printf("原始数据:%s\n",buff.getdata());//写入成功
	return 0;
	
}
