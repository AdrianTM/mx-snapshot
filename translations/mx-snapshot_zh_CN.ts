<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>Batchprocessing</name>
    <message>
        <location filename="../batchprocessing.cpp" line="44"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="45"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>当前内核不支持选择的压缩算法，请编辑配置文件并选择不同的算法。</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="71"/>
        <source>The program will pause the build and open the boot menu in your text editor.</source>
        <translation>程序将暂停构建并在你的文本编辑器中打开启动菜单。</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="101"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="111"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="114"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.ui" line="14"/>
        <location filename="../mainwindow.cpp" line="185"/>
        <location filename="../mainwindow.cpp" line="520"/>
        <source>MX Snapshot</source>
        <translation>MX Snapshot</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="33"/>
        <source>Optional customization</source>
        <translation>可选定制项</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="65"/>
        <source>Release version:</source>
        <translation>发布版本：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="79"/>
        <source>Boot options:</source>
        <translation>启动选项：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="86"/>
        <source>Live kernel:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="93"/>
        <source>Project name:</source>
        <translation>项目名称：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="100"/>
        <source>Release date:</source>
        <translation>发布日期：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="107"/>
        <source>Release codename:</source>
        <translation>发布代号：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="133"/>
        <source>Current date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot is a utility that creates a bootable image (ISO) of your working system that you can use for storage or distribution. You can continue working with undemanding applications while it is running.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot 是一种实用工具，可以为您的工作系统创建可引导镜像 (ISO)，您可以将其用于存储或分发。您可以在运行时继续使用要求不高的应用程序。&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="182"/>
        <source>Used space on / (root) and /home partitions:</source>
        <translation>/ (root) 和 /home 分区上的已用空间：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="192"/>
        <source>Location and ISO name</source>
        <translation>位置和 ISO 名称</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="219"/>
        <source>Snapshot location:</source>
        <translation>快照位置：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="226"/>
        <source>Select a different snapshot directory</source>
        <translation>选择不同的快照目录</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="236"/>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>Snapshot name:</source>
        <translation>快照名称：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="360"/>
        <source>Type of snapshot:</source>
        <translation>快照类型：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="367"/>
        <source>Preserving accounts (for personal backup)</source>
        <translation>保留帐户（用于个人备份）</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="377"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option will reset &amp;quot;demo&amp;quot; and &amp;quot;root&amp;quot; passwords to the MX Linux defaults and will not copy any personal accounts created.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;此选项会将“demo”和“root”密码重置为 MX Linux 默认值，并且不会复制任何已创建的个人帐户。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="380"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>重置帐户（用于分发给他人）</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="451"/>
        <source>You can also exclude certain directories by ticking the common choices below, or by clicking on the button to directly edit /etc/mx-snapshot-exclude.list.</source>
        <translation>您还可以通过勾选下面的常用选项来排除某些目录，或者通过单击按钮直接编辑 /etc/mx-snapshot-exclude.list。</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="496"/>
        <source>sha512</source>
        <translation>sha512</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="519"/>
        <source>Throttle the I/O input rate by the given percentage.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="525"/>
        <source>I/O throttle:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="532"/>
        <source>Calculate checksums:</source>
        <translation>计算校验和：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="539"/>
        <source>ISO compression scheme:</source>
        <translation>ISO压缩方案：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="546"/>
        <source>Number of CPU cores to use:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="553"/>
        <source>md5</source>
        <translation>md5</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="563"/>
        <source>Options:</source>
        <translation>选项：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="608"/>
        <source>Edit Exclusion File</source>
        <translation>编辑排除文件</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="643"/>
        <source>Pictures</source>
        <translation>图片</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="650"/>
        <source>Videos</source>
        <translation>视频</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="657"/>
        <source>All of the above</source>
        <translation>以上全部</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="664"/>
        <source>exclude network configurations</source>
        <translation>排除网络配置</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="667"/>
        <source>Networks</source>
        <translation>网络</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="674"/>
        <source>Downloads</source>
        <translation>下载</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="681"/>
        <source>Desktop</source>
        <translation>桌面</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="688"/>
        <source>Documents</source>
        <translation>文档</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="695"/>
        <source>Flatpaks</source>
        <translation>Flatpaks</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="702"/>
        <source>Music</source>
        <translation>音乐</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="814"/>
        <source>About this application</source>
        <translation>关于此软件</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="817"/>
        <source>About...</source>
        <translation>关于…</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="823"/>
        <source>Alt+B</source>
        <translation>Alt+B</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="839"/>
        <source>Next</source>
        <translation>下一页</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="864"/>
        <source>Display help </source>
        <translation>显示帮助</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="867"/>
        <source>Help</source>
        <translation>帮助</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="873"/>
        <source>Alt+H</source>
        <translation>Alt+H</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="886"/>
        <source>Quit application</source>
        <translation>退出应用程序</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="889"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="895"/>
        <source>Alt+N</source>
        <translation>Alt+N</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="949"/>
        <source>Back</source>
        <translation>后退</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="147"/>
        <source>Select Release Date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fastest, worst compression</source>
        <translation>最快，最差的压缩</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fast, worse compression</source>
        <translation>快速，较差的压缩</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>slow, better compression</source>
        <translation>慢，更好的压缩</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>best compromise</source>
        <translation>最好的这种</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="193"/>
        <source>slowest, best compression</source>
        <translation>最慢，最好的压缩</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="222"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>%1 上的可用空间，其中放置快照文件夹:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="225"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space by removing previous snapshots and saved copies: %1 snapshots are taking up %2 of disk space.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="237"/>
        <location filename="../mainwindow.cpp" line="238"/>
        <source>Installing </source>
        <translation>正在安装</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="323"/>
        <source>Please wait.</source>
        <translation>请稍等。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="325"/>
        <source>Please wait. Calculating used disk space...</source>
        <translation>请稍等。正在计算已用磁盘空间...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="359"/>
        <location filename="../mainwindow.cpp" line="416"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="360"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>输出文件 %1 已经存在。请使用其他文件名，或删除现有文件。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="365"/>
        <source>Settings</source>
        <translation>设置</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="371"/>
        <source>Snapshot will use the following settings:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="373"/>
        <source>- Snapshot directory:</source>
        <translation>- 快照目录：</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>- Kernel to be used:</source>
        <translation>- 要使用的内核：</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="396"/>
        <location filename="../mainwindow.cpp" line="405"/>
        <source>NVIDIA Detected</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="397"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="401"/>
        <source>NVIDIA Selected</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="402"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="406"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="417"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>当前内核不支持选择的压缩算法，请编辑配置文件并选择不同的算法。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="441"/>
        <source>Final chance</source>
        <translation>最后确认</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="443"/>
        <source>Snapshot now has all the information it needs to create an ISO from your running system.</source>
        <translation>Snapshot 现在拥有从正在运行的系统创建 ISO 所需的所有信息。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="444"/>
        <source>It will take some time to finish, depending on the size of the installed system and the capacity of your computer.</source>
        <translation>这将需要一些时间才能完成，具体时长取决于已安装系统的大小和计算机的容量。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="446"/>
        <source>OK to start?</source>
        <translation>准备好要开始了吗？</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="450"/>
        <source>Shutdown computer when done.</source>
        <translation>完成后关闭计算机。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="483"/>
        <source>Output</source>
        <translation>输出</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="499"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="506"/>
        <source>Edit Boot Menu</source>
        <translation>编辑启动菜单</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="507"/>
        <source>The program will now pause to allow you to edit any files in the work directory. Select Yes to edit the boot menu or select No to bypass this step and continue creating the snapshot.</source>
        <translation>程序现在将暂停以允许您编辑工作目录中的任何文件。选择“是”来编辑启动菜单或选择“否”绕过此步骤并继续创建快照。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="603"/>
        <source>About %1</source>
        <translation>关于 %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="605"/>
        <source>Version: </source>
        <translation>版本:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="606"/>
        <source>Program for creating a live-CD from the running system for MX Linux</source>
        <translation>从正在运行的 MX Linux 系统创建 live-CD 的程序</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="608"/>
        <source>Copyright (c) MX Linux</source>
        <translation>版权所有(c) MX Linux</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="610"/>
        <source>%1 License</source>
        <translation>%1 许可证</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="624"/>
        <source>%1 Help</source>
        <translation>%1 帮助</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="629"/>
        <source>Select Snapshot Directory</source>
        <translation>选择快照目录</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Confirmation</source>
        <translation>确认</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Are you sure you want to quit the application?</source>
        <translation>您确定要退出吗？</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="71"/>
        <source>Tool used for creating a live-CD from the running system</source>
        <translation>用于从正在运行的系统创建 live-CD 的工具</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="75"/>
        <source>Use CLI only</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="79"/>
        <source>Number of CPU cores to be used.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="80"/>
        <source>Output directory</source>
        <translation>输出目录</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="81"/>
        <source>Output filename</source>
        <translation>输出文件名</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="83"/>
        <source>Name a different kernel to use other than the default running kernel, use format returned by &apos;uname -r&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="85"/>
        <source>Or the full path: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="88"/>
        <source>Compression level options.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="89"/>
        <source>Use quotes: &quot;-Xcompression-level &lt;level&gt;&quot;, or &quot;-Xalgorithm &lt;algorithm&gt;&quot;, or &quot;-Xhc&quot;, see mksquashfs man page</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="93"/>
        <source>Create a monthly snapshot, add &apos;Month&apos; name in the ISO name, skip used space calculation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="94"/>
        <source>This option sets reset-accounts and compression to defaults, arguments changing those items will be ignored</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="97"/>
        <source>Don&apos;t calculate checksums for resulting ISO file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="98"/>
        <source>Skip calculating free space to see if the resulting ISO will fit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="99"/>
        <source>Option to fix issue with calculating checksums on preempt_rt kernels</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="100"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>重置帐户（用于分发给他人）</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="101"/>
        <source>Calculate checksums for resulting ISO file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="103"/>
        <source>Throttle the I/O input rate by the given percentage. This can be used to reduce the I/O and CPU consumption of Mksquashfs.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="106"/>
        <source>Work directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="108"/>
        <source>Exclude main folders, valid choices: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="110"/>
        <source>Use the option one time for each item you want to exclude</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="113"/>
        <source>Compression format, valid choices: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="115"/>
        <source>Shutdown computer when done.</source>
        <translation>完成后关闭计算机。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="144"/>
        <source>You seem to be logged in as root, please log out and log in as normal user to use this program.</source>
        <translation>您似乎是以 root 身份登录的，请注销并以普通用户身份登录以使用该程序。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="169"/>
        <source>version:</source>
        <translation>版本：</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="163"/>
        <source>You must run this program with sudo or pkexec.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../main.cpp" line="139"/>
        <source>MX Snapshot</source>
        <translation>MX Snapshot</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="148"/>
        <location filename="../main.cpp" line="219"/>
        <location filename="../settings.cpp" line="239"/>
        <location filename="../settings.cpp" line="254"/>
        <location filename="../settings.cpp" line="750"/>
        <location filename="../settings.cpp" line="849"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="216"/>
        <location filename="../settings.cpp" line="248"/>
        <source>Current kernel doesn&apos;t support Squashfs, cannot continue.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="85"/>
        <source>Could not create working directory. </source>
        <translation>无法创建工作目录。</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="109"/>
        <source>Could not create temp directory. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="233"/>
        <source>Could not find a usable kernel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="357"/>
        <source>Used space on / (root): </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="360"/>
        <source>estimated</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="369"/>
        <source>Used space on /home: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="463"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>%1 上的可用空间，其中放置快照文件夹:</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="467"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space
      by removing previous snapshots and saved copies:
      %1 snapshots are taking up %2 of disk space.
</source>
        <translation>可用空间应足以容纳来自 / 和 /home 的压缩数据

  如有必要，您可以创建更多可用空间
  通过删除以前的快照和保存的副本：
  %1 个快照占用了 %2 的磁盘空间。</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="485"/>
        <source>Desktop</source>
        <translation>桌面</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="486"/>
        <source>Documents</source>
        <translation>文档</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="487"/>
        <source>Downloads</source>
        <translation>下载</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="488"/>
        <source>Flatpaks</source>
        <translation>Flatpaks</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="489"/>
        <source>Music</source>
        <translation>音乐</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="490"/>
        <source>Networks</source>
        <translation>网络</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="491"/>
        <source>Pictures</source>
        <translation>图片</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="493"/>
        <source>Videos</source>
        <translation>视频</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="743"/>
        <location filename="../settings.cpp" line="842"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>输出文件 %1 已经存在。请使用其他文件名，或删除现有文件。</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="70"/>
        <source>License</source>
        <translation>许可证</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="71"/>
        <location filename="../about.cpp" line="81"/>
        <source>Changelog</source>
        <translation>更新日志</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="72"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="94"/>
        <source>&amp;Close</source>
        <translation>关闭(&amp;C)</translation>
    </message>
</context>
<context>
    <name>Work</name>
    <message>
        <location filename="../work.cpp" line="82"/>
        <source>Cleaning...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="93"/>
        <location filename="../work.cpp" line="283"/>
        <source>Done</source>
        <translation>完成</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="105"/>
        <source>Interrupted or failed to complete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="138"/>
        <location filename="../work.cpp" line="241"/>
        <location filename="../work.cpp" line="263"/>
        <location filename="../work.cpp" line="301"/>
        <location filename="../work.cpp" line="439"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="139"/>
        <source>There&apos;s not enough free space on your target disk, you need at least %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="142"/>
        <source>You have %1 free space on %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="145"/>
        <source>If you are sure you have enough free space rerun the program with -o/--override-size option</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="170"/>
        <source>Copying the new-iso filesystem...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="182"/>
        <source>Could not create temp directory. </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="238"/>
        <source>Squashing filesystem...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="242"/>
        <source>Could not create linuxfs file, please check /var/log/%1.log</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="260"/>
        <source>Creating CD/DVD image file...</source>
        <translation>创建 CD/DVD 镜像文件...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="264"/>
        <source>Could not create ISO file, please check whether you have enough space on the destination partition.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="270"/>
        <source>Making hybrid iso</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="288"/>
        <source>Success</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="289"/>
        <source>MX Snapshot completed successfully!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="290"/>
        <source>Snapshot took %1 to finish.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="291"/>
        <source>Thanks for using MX Snapshot, run MX Live USB Maker next!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="298"/>
        <source>Installing </source>
        <translation>正在安装</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="301"/>
        <source>Could not install </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="311"/>
        <source>Calculating checksum...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="349"/>
        <source>Building new initrd...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="440"/>
        <source>Could not create working directory. </source>
        <translation>无法创建工作目录。</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="578"/>
        <source>Calculating total size of excluded files...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="588"/>
        <source>Calculating size of root...</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
