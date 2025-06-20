<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fi">
<context>
    <name>Batchprocessing</name>
    <message>
        <location filename="../batchprocessing.cpp" line="44"/>
        <source>Error</source>
        <translation>Virhe</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="45"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>Nykyinen kerneli ei tue valittua pakkausalgoritmia, muokkaa asetustiedostoa ja valitse eri algoritmi.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="71"/>
        <source>The program will pause the build and open the boot menu in your text editor.</source>
        <translation>Ohjelma pysäyttää koontiversion ja avaa käynnistysvalikon tekstieditorissa.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="101"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>Tietokone käyttää Nvidia näytönohjainta. Käytätkö tuloksena olevaa ISO:a tässä tietokoneessa vai toisessa tietokoneessa, jossa on Nvidia?</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="111"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>Huomaa: Jos käytät tuloksena olevaa ISO:a tietokoneessa, jossa ei ole nvidiaa, joudut todennäköisesti poistamaan &quot;xorg=nvidia&quot; käynnistyksen asetuksista.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="114"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>Huomaa: Jos käytät tuloksena olevaa ISO:a tietokoneessa, jossa on nvidia, saatat joutua lisäämään &quot;xorg=nvidia&quot; käynnistyksen asetuksiin.</translation>
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
        <translation>Valinnainen mukautus</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="65"/>
        <source>Release version:</source>
        <translation>Julkaisuversio:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="79"/>
        <source>Boot options:</source>
        <translation>Käynnistyksen valinnat:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="86"/>
        <source>Live kernel:</source>
        <translation>Live kerneli:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="93"/>
        <source>Project name:</source>
        <translation>Projektin nimi:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="100"/>
        <source>Release date:</source>
        <translation>Julkaisupäivä:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="107"/>
        <source>Release codename:</source>
        <translation>Julkaisun nimi:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="133"/>
        <source>Current date</source>
        <translation>Nykyinen päivämäärä</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot is a utility that creates a bootable image (ISO) of your working system that you can use for storage or distribution. You can continue working with undemanding applications while it is running.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot on apuohjelma joka luo käynnistyvän levykuvan (ISO) toimivasta järjestelmästäsi jota voit tallentaa tai jakaa sitä toisille. Voit käyttää järjestelmää normaalisti levykuvan luomisen aikana.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="182"/>
        <source>Used space on / (root) and /home partitions:</source>
        <translation>Käytetty tila / (root) ja /home osioissa:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="192"/>
        <source>Location and ISO name</source>
        <translation>Paikka ja nimi ISO-tiedostolle</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="219"/>
        <source>Snapshot location:</source>
        <translation>Tilannevedoksen paikka:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="226"/>
        <source>Select a different snapshot directory</source>
        <translation>Valitse toinen tilannevedosten hakemisto</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="236"/>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>Snapshot name:</source>
        <translation>Tilannevedoksen nimi:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="360"/>
        <source>Type of snapshot:</source>
        <translation>Tilannevedoksen tyyppi:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="367"/>
        <source>Preserving accounts (for personal backup)</source>
        <translation>Säilytä käyttäjätilit (henkilökohtaista varmuuskopiointia varten)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="377"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option will reset &amp;quot;demo&amp;quot; and &amp;quot;root&amp;quot; passwords to the MX Linux defaults and will not copy any personal accounts created.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tämä valinta nollaa &amp;quot;demo&amp;quot; ja &amp;quot;root&amp;quot; salasanat MX:n vakiollisiin eikä kopioi mitään henkilökohtaisia luotuja tilejä.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="380"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Käyttäjätilien tyhjennys (toisille jakamista varten)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="451"/>
        <source>You can also exclude certain directories by ticking the common choices below, or by clicking on the button to directly edit /etc/mx-snapshot-exclude.list.</source>
        <translation>Voit myös rajata pois hakemistoja ruksaamalla allaolevat yleiset valinnat tai painamalla painiketta ja muokata suoraan tiedostoa  /etc/mx-snapshot-exclude.list.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="496"/>
        <source>sha512</source>
        <translation>sha512</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="519"/>
        <source>Throttle the I/O input rate by the given percentage.</source>
        <translation>Ohjaa I/O-nopeutta annetulla prosentilla.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="525"/>
        <source>I/O throttle:</source>
        <translation>I/O-kuristin:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="532"/>
        <source>Calculate checksums:</source>
        <translation>Laske tarkistussumma:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="539"/>
        <source>ISO compression scheme:</source>
        <translation>ISO-pakkausrakenne:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="546"/>
        <source>Number of CPU cores to use:</source>
        <translation>Käytettävä prosessoriytimien määrä:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="553"/>
        <source>md5</source>
        <translation>md5</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="563"/>
        <source>Options:</source>
        <translation>Asetukset:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="608"/>
        <source>Edit Exclusion File</source>
        <translation>Muokkaa rajaustiedostoa</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="643"/>
        <source>Pictures</source>
        <translation>Kuvat</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="650"/>
        <source>Videos</source>
        <translation>Videot</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="657"/>
        <source>All of the above</source>
        <translation>Kaikki yläpuolella olevat</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="664"/>
        <source>exclude network configurations</source>
        <translation>jätä verkkomääritykset pois</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="667"/>
        <source>Networks</source>
        <translation>Verkot</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="674"/>
        <source>Downloads</source>
        <translation>Lataukset</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="681"/>
        <source>Desktop</source>
        <translation>Työpöytä</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="688"/>
        <source>Documents</source>
        <translation>Tiedostot</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="695"/>
        <source>Flatpaks</source>
        <translation>Flatpakit</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="702"/>
        <source>Music</source>
        <translation>Musiikki</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="814"/>
        <source>About this application</source>
        <translation>Tietoja tästä sovelluksesta</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="817"/>
        <source>About...</source>
        <translation>Tietoja...</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="823"/>
        <source>Alt+B</source>
        <translation>Alt+B</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="839"/>
        <source>Next</source>
        <translation>Seuraava</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="864"/>
        <source>Display help </source>
        <translation>Näytä ohje</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="867"/>
        <source>Help</source>
        <translation>Ohje</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="873"/>
        <source>Alt+H</source>
        <translation>Alt+H</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="886"/>
        <source>Quit application</source>
        <translation>Lopeta sovellus</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="889"/>
        <source>Cancel</source>
        <translation>Peru</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="895"/>
        <source>Alt+N</source>
        <translation>Alt+N</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="949"/>
        <source>Back</source>
        <translation>Takaisin</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="147"/>
        <source>Select Release Date</source>
        <translation>Valitse julkaisupäivä</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fastest, worst compression</source>
        <translation>nopein, huonoin pakkaus</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fast, worse compression</source>
        <translation>nopea, huonompi pakkaus</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>slow, better compression</source>
        <translation>hidas, parempi pakkaus</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>best compromise</source>
        <translation>paras kompromissi</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="193"/>
        <source>slowest, best compression</source>
        <translation>hitain, paras pakkaus</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="222"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>Vapaata tilaa %1, johon tilannevedos on sijoitettu:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="225"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space by removing previous snapshots and saved copies: %1 snapshots are taking up %2 of disk space.</source>
        <translation>Vapaan tilan pitäisi riittää pakatulle datalle kohteista / ja /home 

Tarvittaessa voit tehdä vapaata tilaa poistamalla aiemmat
tilannevedokset ja tallennetut kopiot: Vedokset %1 vievät %2 levytilaa.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="237"/>
        <location filename="../mainwindow.cpp" line="238"/>
        <source>Installing </source>
        <translation>Asennetaan</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="323"/>
        <source>Please wait.</source>
        <translation>Odota, ole hyvä.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="325"/>
        <source>Please wait. Calculating used disk space...</source>
        <translation>Odota, ole hyvä. Lasketaan käytettyä levytilaa...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="359"/>
        <location filename="../mainwindow.cpp" line="416"/>
        <source>Error</source>
        <translation>Virhe</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="360"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>Tiedosto %1 on jo olemassa. Käytä toista tiedostonimeä tai poista olemassa oleva tiedosto.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="365"/>
        <source>Settings</source>
        <translation>Asetukset</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="371"/>
        <source>Snapshot will use the following settings:</source>
        <translation>Tilannevedos käyttää seuraavia asetuksia:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="373"/>
        <source>- Snapshot directory:</source>
        <translation>- Tilannevedosten hakemisto:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>- Kernel to be used:</source>
        <translation>- Käytettävä kerneli:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="396"/>
        <location filename="../mainwindow.cpp" line="405"/>
        <source>NVIDIA Detected</source>
        <translation>Nvidia havaittu</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="397"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>Tietokone käyttää Nvidia näytönohjainta. Käytätkö tuloksena olevaa ISO:a tässä tietokoneessa vai toisessa tietokoneessa, jossa on Nvidia?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="401"/>
        <source>NVIDIA Selected</source>
        <translation>Nvidia valittu</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="402"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>Huomaa: Jos käytät tuloksena olevaa ISO:a tietokoneessa, jossa ei ole nvidiaa, joudut todennäköisesti poistamaan &quot;xorg=nvidia&quot; käynnistyksen asetuksista.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="406"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>Huomaa: Jos käytät tuloksena olevaa ISO:a tietokoneessa, jossa on nvidia, saatat joutua lisäämään &quot;xorg=nvidia&quot; käynnistyksen asetuksiin.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="417"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>Nykyinen kerneli ei tue valittua pakkausalgoritmia, muokkaa asetustiedostoa ja valitse eri algoritmi.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="441"/>
        <source>Final chance</source>
        <translation>Viimeinen mahdollisuus</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="443"/>
        <source>Snapshot now has all the information it needs to create an ISO from your running system.</source>
        <translation>Tilannevedos työkalulla on nyt kaikki tarvittava tieto, jotta se voi tehdä ISO-levykuvan käynnissä olevasta järjestelmästäsi.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="444"/>
        <source>It will take some time to finish, depending on the size of the installed system and the capacity of your computer.</source>
        <translation>Viimeistely tulee kestämään kotvasen aikaa, riippuen asennetun järjestelmän koosta sekä tietokoneesi kapasiteetista.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="446"/>
        <source>OK to start?</source>
        <translation>Voidaanko aloittaa?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="450"/>
        <source>Shutdown computer when done.</source>
        <translation>Sammuta tietokone, kun valmis.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="483"/>
        <source>Output</source>
        <translation>Tuloste</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="499"/>
        <source>Close</source>
        <translation>Sulje</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="506"/>
        <source>Edit Boot Menu</source>
        <translation>Muokkaa käynnistysvalikkoa</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="507"/>
        <source>The program will now pause to allow you to edit any files in the work directory. Select Yes to edit the boot menu or select No to bypass this step and continue creating the snapshot.</source>
        <translation>Tämä ohjelma tauotetaan nyt jotta voit muokata työkansion mitä tahansa tiedostoja. Valitse Kyllä muokataksesi käynnistysvalikkoa, tai valitse vaihtoehtoisesti Ei ohittaaksesi tämän vaiheen sekä jatkaaksesi snapshot-järjestelmävedoksen luomista.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="603"/>
        <source>About %1</source>
        <translation>%1 lisätietoja</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="605"/>
        <source>Version: </source>
        <translation>Versio: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="606"/>
        <source>Program for creating a live-CD from the running system for MX Linux</source>
        <translation>Ohjelma jonka avulla voit luoda live-CD:n tällä hetkellä ajetusta järjestelmästä MX Linux:ille</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="608"/>
        <source>Copyright (c) MX Linux</source>
        <translation>Copyright (c) MX Linux</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="610"/>
        <source>%1 License</source>
        <translation>%1 lupa</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="624"/>
        <source>%1 Help</source>
        <translation>%1 Apuopas</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="629"/>
        <source>Select Snapshot Directory</source>
        <translation>Valitse tilannevedosten hakemisto</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Confirmation</source>
        <translation>Vahvistus</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Are you sure you want to quit the application?</source>
        <translation>Oletko varma että haluat lopettaa sovelluksen?</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="71"/>
        <source>Tool used for creating a live-CD from the running system</source>
        <translation>Ohjelma, jolla luodaan live-CD-levy käynnissä olevasta järjestelmästä</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="75"/>
        <source>Use CLI only</source>
        <translation>Käytä vain CLI</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="79"/>
        <source>Number of CPU cores to be used.</source>
        <translation>Käytettävä prosessoriytimien määrä.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="80"/>
        <source>Output directory</source>
        <translation>Kohde hakemisto</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="81"/>
        <source>Output filename</source>
        <translation>Kohteen tiedostonimi</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="83"/>
        <source>Name a different kernel to use other than the default running kernel, use format returned by &apos;uname -r&apos;</source>
        <translation>Nimeä toinen kernel käytettäväksi kuin oletuksena oleva kernel, käytä &quot;uname -r&quot; palauttamaa muotoa</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="85"/>
        <source>Or the full path: %1</source>
        <translation>tai täydellinen polku: %1</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="88"/>
        <source>Compression level options.</source>
        <translation>Pakkaustason valinta.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="89"/>
        <source>Use quotes: &quot;-Xcompression-level &lt;level&gt;&quot;, or &quot;-Xalgorithm &lt;algorithm&gt;&quot;, or &quot;-Xhc&quot;, see mksquashfs man page</source>
        <translation>Käytä lainausmerkkejä: &quot;-Compression-level &lt;level&gt;&quot;, &quot;-Algoritm &lt;algorithm&gt;&quot; tai &quot;-Xhc&quot;, katso tarvittaessa mksquashfs man-ohjetta</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="93"/>
        <source>Create a monthly snapshot, add &apos;Month&apos; name in the ISO name, skip used space calculation</source>
        <translation>Tee tilannekuva kuukausittain, lisää ISO-nimeen kuluva &quot;kuukausi&quot;, ohita käytetyn tilan laskenta</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="94"/>
        <source>This option sets reset-accounts and compression to defaults, arguments changing those items will be ignored</source>
        <translation>Tämä valinta asettaa palautustilit ja pakkauksen oletuksiin. Kohteita muuttavat argumentit ohitetaan</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="97"/>
        <source>Don&apos;t calculate checksums for resulting ISO file</source>
        <translation>Älä laske valmistuvan ISO-tiedoston tarkistussummaa</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="98"/>
        <source>Skip calculating free space to see if the resulting ISO will fit</source>
        <translation>Ohita vapaan tilan laskeminen näkemättä, sopiiko tuloksena oleva ISO</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="99"/>
        <source>Option to fix issue with calculating checksums on preempt_rt kernels</source>
        <translation>Vaihtoehto, joka korjaa ongelman, joka koskee tarkistussummia preempt_rt kerneleissä</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="100"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Käyttäjätilien tyhjennys (toisille jakamista varten)</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="101"/>
        <source>Calculate checksums for resulting ISO file</source>
        <translation>Laske valmistuvan ISO-tiedoston tarkistussumma</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="103"/>
        <source>Throttle the I/O input rate by the given percentage. This can be used to reduce the I/O and CPU consumption of Mksquashfs.</source>
        <translation>Kurista I/O-nopeutta annetulla prosentilla. Tätä voidaan käyttää vähentämään Mksquashfs I/O ja prosessorin varaamaa tehoa.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="106"/>
        <source>Work directory</source>
        <translation>Työkansio</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="108"/>
        <source>Exclude main folders, valid choices: </source>
        <translation>Jätä pois pääkansiot, vastaavat vaihtoehdot:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="110"/>
        <source>Use the option one time for each item you want to exclude</source>
        <translation>Käytä valintaa kerran kullekin kohteelle, jonka haluat sulkea pois </translation>
    </message>
    <message>
        <location filename="../main.cpp" line="113"/>
        <source>Compression format, valid choices: </source>
        <translation>Pakkausen formaatti, vastaavat vaihtoehdot:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="115"/>
        <source>Shutdown computer when done.</source>
        <translation>Sammuta tietokone, kun valmis.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="144"/>
        <source>You seem to be logged in as root, please log out and log in as normal user to use this program.</source>
        <translation>Olet kirjautunut sisään pääkäyttäjänä. Kirjaudu sisään normaalina käyttäjänä käyttääksesi tätä ohjelmaa.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="169"/>
        <source>version:</source>
        <translation>versio:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="163"/>
        <source>You must run this program with sudo or pkexec.</source>
        <translation>Sinun on suoritettava tämä ohjelma sudo:lla tai pkexec:llä.</translation>
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
        <translation>Virhe</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="216"/>
        <location filename="../settings.cpp" line="248"/>
        <source>Current kernel doesn&apos;t support Squashfs, cannot continue.</source>
        <translation>Nykyinen kerneli ei tue Squashfs:ää, jatkaminen ei onnistu.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="85"/>
        <source>Could not create working directory. </source>
        <translation>Työkansiota ei voitu luoda.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="109"/>
        <source>Could not create temp directory. </source>
        <translation>Tilapäiskansiota ei voitu luoda.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="233"/>
        <source>Could not find a usable kernel</source>
        <translation>Ei löydetty sopivaa kerneliä</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="357"/>
        <source>Used space on / (root): </source>
        <translation>Käytetty tila kohteessa / (root-juurihakemisto):</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="360"/>
        <source>estimated</source>
        <translation>arvioitu</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="369"/>
        <source>Used space on /home: </source>
        <translation>Käytetty tila /home -hakemistossa: </translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="463"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>Vapaata tilaa %1, johon tilannevedos on sijoitettu:</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="467"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space
      by removing previous snapshots and saved copies:
      %1 snapshots are taking up %2 of disk space.
</source>
        <translation>Vapaan tilan määrä pitäisi olla riittävä pakatulle datalle kohteista / ja /home 

            Mikäli välttämätöntä, voit luoda lisää tilaa käytettäväksi
            poistamalla edellisiä snapshot-järjestelmävedoksia sekä jäljennöksiä:
            %1 snapshot-järjestelmävedokset vievät %2 levytilaa.
</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="485"/>
        <source>Desktop</source>
        <translation>Työpöytä</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="486"/>
        <source>Documents</source>
        <translation>Tiedostot</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="487"/>
        <source>Downloads</source>
        <translation>Lataukset</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="488"/>
        <source>Flatpaks</source>
        <translation>Flatpakit</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="489"/>
        <source>Music</source>
        <translation>Musiikki</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="490"/>
        <source>Networks</source>
        <translation>Verkot</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="491"/>
        <source>Pictures</source>
        <translation>Kuvat</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="493"/>
        <source>Videos</source>
        <translation>Videot</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="743"/>
        <location filename="../settings.cpp" line="842"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>Tiedosto %1 on jo olemassa. Käytä toista tiedostonimeä tai poista olemassa oleva tiedosto.</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="70"/>
        <source>License</source>
        <translation>Lisenssi</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="71"/>
        <location filename="../about.cpp" line="81"/>
        <source>Changelog</source>
        <translation>Muutosloki</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="72"/>
        <source>Cancel</source>
        <translation>Peru</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="94"/>
        <source>&amp;Close</source>
        <translation>&amp;Sulje</translation>
    </message>
</context>
<context>
    <name>Work</name>
    <message>
        <location filename="../work.cpp" line="82"/>
        <source>Cleaning...</source>
        <translation>Puhdistetaan...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="93"/>
        <location filename="../work.cpp" line="283"/>
        <source>Done</source>
        <translation>Valmis</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="105"/>
        <source>Interrupted or failed to complete</source>
        <translation>Keskeytetty tai epäonnistunut</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="138"/>
        <location filename="../work.cpp" line="241"/>
        <location filename="../work.cpp" line="263"/>
        <location filename="../work.cpp" line="301"/>
        <location filename="../work.cpp" line="439"/>
        <source>Error</source>
        <translation>Virhe</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="139"/>
        <source>There&apos;s not enough free space on your target disk, you need at least %1</source>
        <translation>Levyllä ei ole tarpeeksi vapaata tilaa, tarvitset vähintään %1</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="142"/>
        <source>You have %1 free space on %2</source>
        <translation>Sinulla on %1 vapaata tilaa kohteessa %2</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="145"/>
        <source>If you are sure you have enough free space rerun the program with -o/--override-size option</source>
        <translation>Jos olet varma, että sinulla on tarpeeksi vapaata tilaa, suorita ohjelma uudelleen -o/--override-size vailinnalla</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="170"/>
        <source>Copying the new-iso filesystem...</source>
        <translation>Jäljennetään new-iso tiedostojärjestelmää...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="182"/>
        <source>Could not create temp directory. </source>
        <translation>Tilapäiskansiota ei voitu luoda.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="238"/>
        <source>Squashing filesystem...</source>
        <translation>Puristetaan tiedostojärjestelmää...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="242"/>
        <source>Could not create linuxfs file, please check /var/log/%1.log</source>
        <translation>Ei voitu luoda linuxfs-tiedostoa, tarkista /var/log/%1.log</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="260"/>
        <source>Creating CD/DVD image file...</source>
        <translation>CD/DVD-levykuvatiedostoa luodaan...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="264"/>
        <source>Could not create ISO file, please check whether you have enough space on the destination partition.</source>
        <translation>ISO-tiedostoa ei voitu luoda, tarkista onko kohdeosiolla tarpeeksi levytilaa.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="270"/>
        <source>Making hybrid iso</source>
        <translation>Tuotetaan risteytetty ISO</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="288"/>
        <source>Success</source>
        <translation>Onnistui</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="289"/>
        <source>MX Snapshot completed successfully!</source>
        <translation>MX Snapshot valmistui onnistuneesti!</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="290"/>
        <source>Snapshot took %1 to finish.</source>
        <translation>Snapshot valmistuminen kesti %1.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="291"/>
        <source>Thanks for using MX Snapshot, run MX Live USB Maker next!</source>
        <translation>Kiitos kun käytit MX Snapshot työkalua, aja MX Live USB Maker seuraavaksi!</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="298"/>
        <source>Installing </source>
        <translation>Asennetaan</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="301"/>
        <source>Could not install </source>
        <translation>Ei voitu asentaa</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="311"/>
        <source>Calculating checksum...</source>
        <translation>Lasketaan tarkistussummaa...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="349"/>
        <source>Building new initrd...</source>
        <translation>Rakennetaan uutta initrd:tä...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="440"/>
        <source>Could not create working directory. </source>
        <translation>Työkansiota ei voitu luoda.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="578"/>
        <source>Calculating total size of excluded files...</source>
        <translation>Lasketaan poissuljettujen tiedostojen kokoa...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="588"/>
        <source>Calculating size of root...</source>
        <translation>Lasketaan root kokoa...</translation>
    </message>
</context>
</TS>
