# 在头文件中一般不时候命名空间

# 判断语句出错 之前写成!width_ <= 0
``` c
if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)
    {
        return false;
    }
```

# 判断语句出错 之前写成!re
``` c
if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }
```

# SDL窗口拖拽变化大小
``` c
re = SDL_RenderCopy(render_, texture_, NULL, prect);
```
``` c
SDL_RenderCopy(renderer, texture, srcrect, dstrect)
```
srcrect = NULL → 用整幅 texture；

dstrect = NULL → 把 texture 缩放到当前 renderer 的输出尺寸（=窗口大小），并 忽略你手动拖拽后的窗口尺寸变化——SDL 内部会实时套用到最新宽高。

你把 prect 设成 NULL（即第 4 个参数传 nullptr）以后，不管用户怎么拖拽边框，下一帧就会自动铺满最新窗口，无需你自己再算 scale_w_/scale_h_。

反之，如果你传了非空指针，SDL 就按你给定的矩形画，不会再随窗口大小变化而缩放；窗口变大就会出现黑边，变小则画面被裁剪。