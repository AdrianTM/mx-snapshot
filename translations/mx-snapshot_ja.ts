<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja">
<context>
    <name>Batchprocessing</name>
    <message>
        <location filename="../batchprocessing.cpp" line="44"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="45"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>現在のカーネルは選択された圧縮アルゴリズムをサポートしていないので、設定ファイルを編集して別のアルゴリズムを選択してください。</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="71"/>
        <source>The program will pause the build and open the boot menu in your text editor.</source>
        <translation>このプログラムでは、ビルドを一時停止し、テキストエディタでブートメニューを開きます。</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="101"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>このコンピュータは NVIDIA グラフィックカードを使用しています。出来上がった ISO をこのコンピュータ、または NVIDIA カード搭載の別コンピュータで使用する予定はありますか？</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="111"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>注意。出来上がった ISO を NVIDIA カード非搭載のコンピュータで使う場合、起動オプションの中から &apos;xorg=nvidia&apos; という記述を削除する必要があります。</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="114"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>注意。出来上がった ISO を NVIDIA カード搭載コンピュータで使う場合、起動オプションに &apos;xorg=nvidia&apos; を追加する必要があるかもしれません。</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.ui" line="14"/>
        <location filename="../mainwindow.cpp" line="185"/>
        <location filename="../mainwindow.cpp" line="520"/>
        <source>MX Snapshot</source>
        <translation>MX スナップショット</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="33"/>
        <source>Optional customization</source>
        <translation>カスタムオプション</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="65"/>
        <source>Release version:</source>
        <translation>リリースのバージョン:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="79"/>
        <source>Boot options:</source>
        <translation>起動オプション:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="86"/>
        <source>Live kernel:</source>
        <translation>Live kernel:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="93"/>
        <source>Project name:</source>
        <translation>プロジェクト名:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="100"/>
        <source>Release date:</source>
        <translation>リリースの日付</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="107"/>
        <source>Release codename:</source>
        <translation>リリースのコード名:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="133"/>
        <source>Current date</source>
        <translation>現在の日付</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot is a utility that creates a bootable image (ISO) of your working system that you can use for storage or distribution. You can continue working with undemanding applications while it is running.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;MX スナップショットは、稼働中のシステムからストレージへのインストールまたは他人へ配布する用途で、起動可能なイメージ (ISO) を作製するユーティリティです。少ない手順で作業を進める事ができます。&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="182"/>
        <source>Used space on / (root) and /home partitions:</source>
        <translation>/ (root) および /home パーティションの空き容量を使用します: </translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="192"/>
        <source>Location and ISO name</source>
        <translation>場所と ISO の名称</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="219"/>
        <source>Snapshot location:</source>
        <translation>スナップショットを保存する場所:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="226"/>
        <source>Select a different snapshot directory</source>
        <translation>差分スナップショットのディレクトリを選択</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="236"/>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>Snapshot name:</source>
        <translation>スナップショットのファイル名:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="360"/>
        <source>Type of snapshot:</source>
        <translation>スナップショットの種類:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="367"/>
        <source>Preserving accounts (for personal backup)</source>
        <translation>アカウントの保存 (個人的なバックアップ用途)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="377"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option will reset &amp;quot;demo&amp;quot; and &amp;quot;root&amp;quot; passwords to the MX Linux defaults and will not copy any personal accounts created.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;このオプションでは、MX Linux の「demo」と「root」パスワードを既定にリセットします。また、作成した個人アカウントはコピーしません。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="380"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>アカウントのリセット (他人へ配布する用途)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="451"/>
        <source>You can also exclude certain directories by ticking the common choices below, or by clicking on the button to directly edit /etc/mx-snapshot-exclude.list.</source>
        <translation>以下の一般的な選択肢にチェックを入れるか、またはボタンをクリックして /etc/mx-snapshot-exclude.list を直接編集することによっても、特定のディレクトリを除外することができます。</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="496"/>
        <source>sha512</source>
        <translation>sha512</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="519"/>
        <source>Throttle the I/O input rate by the given percentage.</source>
        <translation>I/O 入力レートを指定したパーセンテージに制御します。</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="525"/>
        <source>I/O throttle:</source>
        <translation>I/O スロットリング:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="532"/>
        <source>Calculate checksums:</source>
        <translation>チェックサムの生成:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="539"/>
        <source>ISO compression scheme:</source>
        <translation>ISO 圧縮方式：</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="546"/>
        <source>Number of CPU cores to use:</source>
        <translation>使用する CPU コア数:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="553"/>
        <source>md5</source>
        <translation>md5</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="563"/>
        <source>Options:</source>
        <translation>オプション:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="608"/>
        <source>Edit Exclusion File</source>
        <translation>除外するファイルを編集</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="643"/>
        <source>Pictures</source>
        <translation>画像</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="650"/>
        <source>Videos</source>
        <translation>動画</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="657"/>
        <source>All of the above</source>
        <translation>上記すべて</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="664"/>
        <source>exclude network configurations</source>
        <translation>ネットワークの設定を除外します</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="667"/>
        <source>Networks</source>
        <translation>ネットワーク</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="674"/>
        <source>Downloads</source>
        <translation>ダウンロード</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="681"/>
        <source>Desktop</source>
        <translation>デスクトップ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="688"/>
        <source>Documents</source>
        <translation>文書</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="695"/>
        <source>Flatpaks</source>
        <translation>Flatpaks</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="702"/>
        <source>Music</source>
        <translation>音楽</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="814"/>
        <source>About this application</source>
        <translation>このアプリケーションについて</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="817"/>
        <source>About...</source>
        <translation>情報...</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="823"/>
        <source>Alt+B</source>
        <translation>Alt+B</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="839"/>
        <source>Next</source>
        <translation>次へ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="864"/>
        <source>Display help </source>
        <translation>ヘルプを表示</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="867"/>
        <source>Help</source>
        <translation>ヘルプ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="873"/>
        <source>Alt+H</source>
        <translation>Alt+H</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="886"/>
        <source>Quit application</source>
        <translation>アプリケーションの終了</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="889"/>
        <source>Cancel</source>
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="895"/>
        <source>Alt+N</source>
        <translation>Alt+N</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="949"/>
        <source>Back</source>
        <translation>戻る</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="147"/>
        <source>Select Release Date</source>
        <translation>リリースの日付を選択</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fastest, worst compression</source>
        <translation>最も高速ですが低圧縮です</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fast, worse compression</source>
        <translation>高速ですがやや低圧縮です</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>slow, better compression</source>
        <translation>低速ですが良く圧縮できます</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>best compromise</source>
        <translation>最もバランスの良い選択です</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="193"/>
        <source>slowest, best compression</source>
        <translation>最も低速ですが高圧縮です</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="222"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>スナップショットのフォルダがある場所の空き容量 %1：</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="225"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space by removing previous snapshots and saved copies: %1 snapshots are taking up %2 of disk space.</source>
        <translation>空き容量は / と /home の圧縮データを保持するのに十分でなくてはなりません。

      必要に応じて、以前作成したスナップショットや保存済みのコピーを削除して、使用可能な領域を増やすことができます。 %1 スナップショットはディスク領域の %2 を占有しています。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="237"/>
        <location filename="../mainwindow.cpp" line="238"/>
        <source>Installing </source>
        <translation>インストール中</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="323"/>
        <source>Please wait.</source>
        <translation>お待ち下さい。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="325"/>
        <source>Please wait. Calculating used disk space...</source>
        <translation>お待ち下さい。使用する空き容量を計算しています...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="359"/>
        <location filename="../mainwindow.cpp" line="416"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="360"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>出力ファイル名 %1 はすでに存在します。別の名前を使用するか、すでにあるそのファイルを削除してください。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="365"/>
        <source>Settings</source>
        <translation>設定中</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="371"/>
        <source>Snapshot will use the following settings:</source>
        <translation>スナップショットでは、今後以下の設定に従います。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="373"/>
        <source>- Snapshot directory:</source>
        <translation>- スナップショットのディレクトリ: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>- Kernel to be used:</source>
        <translation>- 使用するカーネル: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="396"/>
        <location filename="../mainwindow.cpp" line="405"/>
        <source>NVIDIA Detected</source>
        <translation>NVIDIA を検出</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="397"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>このコンピュータは NVIDIA グラフィックカードを使用しています。出来上がった ISO をこのコンピュータ、または NVIDIA カード搭載の別コンピュータで使用する予定はありますか？</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="401"/>
        <source>NVIDIA Selected</source>
        <translation>NVIDIA を選択中</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="402"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>注意。出来上がった ISO を NVIDIA カード非搭載のコンピュータで使う場合、起動オプションの中から &apos;xorg=nvidia&apos; という記述を削除する必要があります。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="406"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>注意。出来上がった ISO を NVIDIA カード搭載コンピュータで使う場合、起動オプションに &apos;xorg=nvidia&apos; を追加する必要があるかもしれません。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="417"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>現在のカーネルは選択された圧縮アルゴリズムをサポートしていないので、設定ファイルを編集して別のアルゴリズムを選択してください。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="441"/>
        <source>Final chance</source>
        <translation>最終確認</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="443"/>
        <source>Snapshot now has all the information it needs to create an ISO from your running system.</source>
        <translation>MX スナップショットには現在、稼働しているシステムから ISO を生成するために必要な全情報を含んでいます。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="444"/>
        <source>It will take some time to finish, depending on the size of the installed system and the capacity of your computer.</source>
        <translation>作業完了までにかかる時間は、インストールを行うシステムのサイズとコンピュータの性能によって異なります。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="446"/>
        <source>OK to start?</source>
        <translation>OK で開始します</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="450"/>
        <source>Shutdown computer when done.</source>
        <translation>完了したらコンピュータをシャットダウンする。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="483"/>
        <source>Output</source>
        <translation>出力</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="499"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="506"/>
        <source>Edit Boot Menu</source>
        <translation>起動メニューの編集</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="507"/>
        <source>The program will now pause to allow you to edit any files in the work directory. Select Yes to edit the boot menu or select No to bypass this step and continue creating the snapshot.</source>
        <translation>プログラムは、作業ディレクトリで任意ファイルを編集できるように、一時休止しています。「はい」を選んで起動メニューを編集するか、「いいえ」を選んでこのステップを飛ばしてスナップショットの作成を継続するかを選んでください。</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="603"/>
        <source>About %1</source>
        <translation> %1 について</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="605"/>
        <source>Version: </source>
        <translation>バージョン: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="606"/>
        <source>Program for creating a live-CD from the running system for MX Linux</source>
        <translation>起動中の MX Linux を元にして Live CD を生成するプログラム</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="608"/>
        <source>Copyright (c) MX Linux</source>
        <translation>Copyright (c) MX Linux</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="610"/>
        <source>%1 License</source>
        <translation>%1 ライセンス</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="624"/>
        <source>%1 Help</source>
        <translation>%1 のヘルプ</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="629"/>
        <source>Select Snapshot Directory</source>
        <translation>スナップショットを保存するディレクトリを選択</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Confirmation</source>
        <translation>確認</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Are you sure you want to quit the application?</source>
        <translation>本当にこのアプリを終了しますか？</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="71"/>
        <source>Tool used for creating a live-CD from the running system</source>
        <translation>稼働中のシステムから Live-CD を作成するツール</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="75"/>
        <source>Use CLI only</source>
        <translation>コマンドラインのみ使用する</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="79"/>
        <source>Number of CPU cores to be used.</source>
        <translation>使用する CPU のコアの数です</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="80"/>
        <source>Output directory</source>
        <translation>出力先のディレクトリ</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="81"/>
        <source>Output filename</source>
        <translation>出力先のファイル名</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="83"/>
        <source>Name a different kernel to use other than the default running kernel, use format returned by &apos;uname -r&apos;</source>
        <translation>別のカーネル名を指定して、既定で動作しているカーネル以外を使用します。なお、&apos;uname -r&apos; が返す書式を利用します。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="85"/>
        <source>Or the full path: %1</source>
        <translation>またはフルパス： %1</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="88"/>
        <source>Compression level options.</source>
        <translation>圧縮レベルのオプション。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="89"/>
        <source>Use quotes: &quot;-Xcompression-level &lt;level&gt;&quot;, or &quot;-Xalgorithm &lt;algorithm&gt;&quot;, or &quot;-Xhc&quot;, see mksquashfs man page</source>
        <translation>引用符を使って、 &quot;-Xcompression-level &lt;level&gt;&quot; または &quot;-Xalgorithm &lt;algorithm&gt;&quot; または &quot;-Xhc&quot; とします。詳しくは mksquashfs のマニュアルページを見てください。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="93"/>
        <source>Create a monthly snapshot, add &apos;Month&apos; name in the ISO name, skip used space calculation</source>
        <translation>月毎のスナップショットの作成。ISO名に月名を追加し、使用済み容量の計算をスキップ。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="94"/>
        <source>This option sets reset-accounts and compression to defaults, arguments changing those items will be ignored</source>
        <translation>このオプションは reset-accounts と compression を既定に設定し、これらの項目を変更する引数は無視します。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="97"/>
        <source>Don&apos;t calculate checksums for resulting ISO file</source>
        <translation>作成した ISO ファイルのチェックサムを計算しない</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="98"/>
        <source>Skip calculating free space to see if the resulting ISO will fit</source>
        <translation>作成する ISO が収まるかどうかの、空き容量計算をスキップする</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="99"/>
        <source>Option to fix issue with calculating checksums on preempt_rt kernels</source>
        <translation>preempt_rt カーネルでのチェックサムに関する問題を修正するためのオプション</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="100"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>アカウントのリセット (他人へ配布する用途向け)</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="101"/>
        <source>Calculate checksums for resulting ISO file</source>
        <translation>作成したISOファイルのチェックサムを算出する</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="103"/>
        <source>Throttle the I/O input rate by the given percentage. This can be used to reduce the I/O and CPU consumption of Mksquashfs.</source>
        <translation>I/O入力レートを指定したパーセンテージに制御します。これは Mksquashfs の I/O と CPU の消費を低減する目的で使用できます。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="106"/>
        <source>Work directory</source>
        <translation>作業用ディレクトリ</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="108"/>
        <source>Exclude main folders, valid choices: </source>
        <translation>メインフォルダの除外、有効にするオプション:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="110"/>
        <source>Use the option one time for each item you want to exclude</source>
        <translation>除外したい項目毎に、１度だけ使用してください</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="113"/>
        <source>Compression format, valid choices: </source>
        <translation>有効にする圧縮形式の選択: </translation>
    </message>
    <message>
        <location filename="../main.cpp" line="115"/>
        <source>Shutdown computer when done.</source>
        <translation>完了したらコンピュータをシャットダウンする。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="144"/>
        <source>You seem to be logged in as root, please log out and log in as normal user to use this program.</source>
        <translation>root ユーザとしてログインしているようです。このプログラムを使用するには、一度ログアウトして通常のユーザとしてログインしてください。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="169"/>
        <source>version:</source>
        <translation>バージョン:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="163"/>
        <source>You must run this program with sudo or pkexec.</source>
        <translation>このプログラムは、sudo または pkexec で実行する必要があります。</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="139"/>
        <source>MX Snapshot</source>
        <translation>MX スナップショット</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="148"/>
        <location filename="../main.cpp" line="219"/>
        <location filename="../settings.cpp" line="239"/>
        <location filename="../settings.cpp" line="254"/>
        <location filename="../settings.cpp" line="750"/>
        <location filename="../settings.cpp" line="849"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="216"/>
        <location filename="../settings.cpp" line="248"/>
        <source>Current kernel doesn&apos;t support Squashfs, cannot continue.</source>
        <translation>現在のカーネルは Squashfs をサポートしてないので続行できません。</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="85"/>
        <source>Could not create working directory. </source>
        <translation>作業用ディレクトリを作成できませんでした。</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="109"/>
        <source>Could not create temp directory. </source>
        <translation>temp ディレクトリを作成できませんでした。</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="233"/>
        <source>Could not find a usable kernel</source>
        <translation>使用可能なカーネルが見つかりません</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="357"/>
        <source>Used space on / (root): </source>
        <translation>/ (root) の空き容量: </translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="360"/>
        <source>estimated</source>
        <translation>推定</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="369"/>
        <source>Used space on /home: </source>
        <translation>/home の空き容量: </translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="463"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>スナップショットを作成するフォルダがある場所の空き容量 %1：</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="467"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space
      by removing previous snapshots and saved copies:
      %1 snapshots are taking up %2 of disk space.
</source>
        <translation>/ および /home の圧縮データを保管するには、十分な空き容量がないといけません。必要に応じて、以前生成したスナップショットや保存したコピーを削除することで空き容量を増やすことができます:&#x3000;スナップショット %1 は空き容量の %2 を消費しています。</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="485"/>
        <source>Desktop</source>
        <translation>デスクトップ</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="486"/>
        <source>Documents</source>
        <translation>文書</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="487"/>
        <source>Downloads</source>
        <translation>ダウンロード</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="488"/>
        <source>Flatpaks</source>
        <translation>Flatpaks</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="489"/>
        <source>Music</source>
        <translation>音楽</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="490"/>
        <source>Networks</source>
        <translation>ネットワーク</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="491"/>
        <source>Pictures</source>
        <translation>画像</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="493"/>
        <source>Videos</source>
        <translation>動画</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="743"/>
        <location filename="../settings.cpp" line="842"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>出力ファイル名 %1 はすでに存在します。別の名前を使用するか、すでにあるそのファイルを削除してください。</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="70"/>
        <source>License</source>
        <translation>ライセンス</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="71"/>
        <location filename="../about.cpp" line="81"/>
        <source>Changelog</source>
        <translation>更新履歴</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="72"/>
        <source>Cancel</source>
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="94"/>
        <source>&amp;Close</source>
        <translation>閉じる(&amp;C)</translation>
    </message>
</context>
<context>
    <name>Work</name>
    <message>
        <location filename="../work.cpp" line="82"/>
        <source>Cleaning...</source>
        <translation>クリーニング中...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="93"/>
        <location filename="../work.cpp" line="283"/>
        <source>Done</source>
        <translation>完了</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="105"/>
        <source>Interrupted or failed to complete</source>
        <translation>中断されたか、または完了できませんでした</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="138"/>
        <location filename="../work.cpp" line="241"/>
        <location filename="../work.cpp" line="263"/>
        <location filename="../work.cpp" line="301"/>
        <location filename="../work.cpp" line="439"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="139"/>
        <source>There&apos;s not enough free space on your target disk, you need at least %1</source>
        <translation>作成するディスクに十分な空き領域がありません。最低でも %1 必要です</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="142"/>
        <source>You have %1 free space on %2</source>
        <translation>%2 に %1 の空き領域があります</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="145"/>
        <source>If you are sure you have enough free space rerun the program with -o/--override-size option</source>
        <translation>充分な空き容量があると分かっているのであれば、-o/--override-size オプションを付けてプログラムを再実行してください。</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="170"/>
        <source>Copying the new-iso filesystem...</source>
        <translation>新たな iso ファイルシステムをコピー中...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="182"/>
        <source>Could not create temp directory. </source>
        <translation>temp ディレクトリを作成できませんでした。</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="238"/>
        <source>Squashing filesystem...</source>
        <translation>ファイルシステムに Squash を適用中...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="242"/>
        <source>Could not create linuxfs file, please check /var/log/%1.log</source>
        <translation>linuxfs ファイルを作成できませんでした。/var/log/%1.log をチェックしてください。</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="260"/>
        <source>Creating CD/DVD image file...</source>
        <translation>CD/DVD イメージファイルを生成中...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="264"/>
        <source>Could not create ISO file, please check whether you have enough space on the destination partition.</source>
        <translation>ISOファイルを作成できませんでした。パーティションに十分な空き容量があるかご確認ください。</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="270"/>
        <source>Making hybrid iso</source>
        <translation>ハイブリット ISO を作成中</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="288"/>
        <source>Success</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="289"/>
        <source>MX Snapshot completed successfully!</source>
        <translation>MX スナップショットの作成に成功しました！</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="290"/>
        <source>Snapshot took %1 to finish.</source>
        <translation>スナップショットの作成が完了するまで %1 かかりました。</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="291"/>
        <source>Thanks for using MX Snapshot, run MX Live USB Maker next!</source>
        <translation>MX スナップショットをご利用いただきありがとうございました。続いて MX Live USB メーカ を起動してください！</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="298"/>
        <source>Installing </source>
        <translation>インストール中</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="301"/>
        <source>Could not install </source>
        <translation>インストールできません</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="311"/>
        <source>Calculating checksum...</source>
        <translation>チェックサムの計算中...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="349"/>
        <source>Building new initrd...</source>
        <translation>新しい initrd を構築中...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="440"/>
        <source>Could not create working directory. </source>
        <translation>作業用ディレクトリを作成できませんでした。</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="578"/>
        <source>Calculating total size of excluded files...</source>
        <translation>除外するファイルの合計サイズを計算中です...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="588"/>
        <source>Calculating size of root...</source>
        <translation>root のサイズを計算中です...</translation>
    </message>
</context>
</TS>
