# yuv_file.open("400_300_25.yuv",ios::binary); yuv必须用二进制格式打开
# 调试→工作目录也要加上路径，否则找不到放在bin/x86下的资源
只改了“输出目录”，没改“调试→命令”
按 F5 调试时，VS 启动的是
调试→工作目录 下的 exe，而不是“输出目录”下的。
打开
项目属性 → 调试 → 工作目录
改成跟“输出目录”一致（或至少指向那里），再按 F5 就能找到。
# 创建texture必须有大小，否则创建出来的是空指针
sdl_render_width = ui.label->width();
sdl_render_height = ui.label->height();
# yuv 格式大小
yuv_file.read(yuv, sdl_render_width * sdl_render_height * 1.5);
YUV420P 每帧大小：
Y：width×height
U：width×height/4
V：width×height/4
合计 width*height*1.5 字节，一次性读进来。
