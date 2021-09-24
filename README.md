# QProcessReadStdOut

qt 启动 ffmpeg 进程，并且读取ffmpeg的转换进度。
从ffmpeg的输出中提取进度，要点是在编码前捕获 Duration 值，并在编码期间捕获 time=... 值。

## 使用说明

需要将 项目中的 `ffpmeg.exe` 以及 `dancing.mp4` 拷贝到应用程序目录下去

## 文章参考

QProcess 实时读取ffmpeg进度 <https://zhuanlan.zhihu.com/p/86206831>
