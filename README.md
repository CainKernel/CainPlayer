# CainPlayer
基于ffplay逻辑实现的简单播放器。

# 支持部分ffplay命令
通过使用 setOption 方法指定参数：
```
指定使用MediaCodec进行解码
mCainMediaPlayer.setOption(CainMediaPlayer.OPT_CATEGORY_PLAYER, "vcodec", "h264_mediacodec");
```

# 截图
![本地视频](https://github.com/CainKernel/CainPlayer/blob/master/screenshot/native_video.png)

![网络视频](https://github.com/CainKernel/CainPlayer/blob/master/screenshot/rtmp_video.png)

# 讲解文章
[第零章 基础公共类的封装](https://www.jianshu.com/p/9003caa6683f)

[第一章 播放器初始化与解复用流程](https://www.jianshu.com/p/95dc19217847)

[第二章 音视解码器和视频解码器实现](https://www.jianshu.com/p/8de0fc796ef9)

[第三章 音频输出 —— OpenSLES](https://www.jianshu.com/p/9b41212c71a5)

[第四章 音频重采样与变速变调处理](https://www.jianshu.com/p/4af5d16ac017)

[第五章 视频同步渲染输出](https://www.jianshu.com/p/f8ba3ceac687)
