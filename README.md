# webserver
</p>这是一个webserver项目，作者目前学生，学做项目准备找工作
这个项目是目前作者的一个学习项目，接下来几个月内会不停修改，如果有对这方面感兴趣的朋友运行了此程序，希望可以给些建议，并给个小红心,本人联系方式：vx:Lv2shaonian。</p>
</p>程序运行：<br>
git clone https://github.com/theblacktree/webserver.git <br>         
    g++ *.cpp -o webs -pthread进行编译<br>
./webs 10000运行可执行文件，指定10000端口号<br>
ifconfig查看自己的ip地址，在任何一台电脑上浏览器上输入http://192.168.xxx.xxx:10000/index.html</p>

</p>2023.5.10第一次修改：修改了locker头文件，仿照locker_guard实现自己的locker_guarder,并在threadpool.h中使用了locker_guarder,创建局部锁类，利用类脱离作用域自动析构的特点实现raii思想的互斥锁，缺点是使用不够灵活，加了if(1)限定作用域。
