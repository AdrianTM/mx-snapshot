<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="it">
<context>
    <name>Batchprocessing</name>
    <message>
        <location filename="../batchprocessing.cpp" line="44"/>
        <source>Error</source>
        <translation>Errore</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="45"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>Il kernel attuale non supporta l&apos;algoritmo di compressione selezionato, prego modifica il file di configurazione e seleziona un algoritmo differente.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="71"/>
        <source>The program will pause the build and open the boot menu in your text editor.</source>
        <translation>Il programma sospenderà la costruzione e aprirà il menu boot nel tuo editor di testo.</translation>
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
        <translation>Personalizzazione opzionale</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="65"/>
        <source>Release version:</source>
        <translation>Versione del rilascio:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="79"/>
        <source>Boot options:</source>
        <translation>Opzioni di avvio:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="86"/>
        <source>Live kernel:</source>
        <translation>Kernel live:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="93"/>
        <source>Project name:</source>
        <translation>Nome del progetto:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="100"/>
        <source>Release date:</source>
        <translation>Data di rilascio:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="107"/>
        <source>Release codename:</source>
        <translation>Nome in codice del rilascio:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="133"/>
        <source>Current date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot is a utility that creates a bootable image (ISO) of your working system that you can use for storage or distribution. You can continue working with undemanding applications while it is running.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot è un&apos;utility che crea un&apos;immagine (ISO) avviabile del tuo attuale sistema che puoi conservare o distribuire. Puoi continuare a lavorare con applicazioni leggere mentre è in esecuzione.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="182"/>
        <source>Used space on / (root) and /home partitions:</source>
        <translation>Spazio utilizzato nelle partizioni / (root) e /home:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="192"/>
        <source>Location and ISO name</source>
        <translation>Nome della posizione e della ISO</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="219"/>
        <source>Snapshot location:</source>
        <translation>Posizione Snapshot</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="226"/>
        <source>Select a different snapshot directory</source>
        <translation>Seleziona una directory diversa per la snapshot</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="236"/>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>Snapshot name:</source>
        <translation>Nome Snapshot:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="360"/>
        <source>Type of snapshot:</source>
        <translation>Tipo di snapshot:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="367"/>
        <source>Preserving accounts (for personal backup)</source>
        <translation>Preservazione degli account (per un backup personale)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="377"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option will reset &amp;quot;demo&amp;quot; and &amp;quot;root&amp;quot; passwords to the MX Linux defaults and will not copy any personal accounts created.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Questa opzione resetterà le passwords &amp;quot;demo&amp;quot; e &amp;quot;root&amp;quot; a quelle di default di MX Linux e non copierà alcun accounts personale creato da te.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="380"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Resettaggio degli account (per distribuire ad altri)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="451"/>
        <source>You can also exclude certain directories by ticking the common choices below, or by clicking on the button to directly edit /etc/mx-snapshot-exclude.list.</source>
        <translation>Puoi anche escludere alcune directory spuntando le scelte comuni di seguito, o facendo clic sul pulsante per modificare direttamente /etc/mx-snapshot-exclude.list.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="496"/>
        <source>sha512</source>
        <translation>sha512</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="519"/>
        <source>Throttle the I/O input rate by the given percentage.</source>
        <translation>Limita la velocità di ingresso dell&apos;I/O della percentuale indicata.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="525"/>
        <source>I/O throttle:</source>
        <translation>Acceleratore I/O:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="532"/>
        <source>Calculate checksums:</source>
        <translation>Calcola checksum:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="539"/>
        <source>ISO compression scheme:</source>
        <translation>Schema di compressione ISO:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="546"/>
        <source>Number of CPU cores to use:</source>
        <translation>Numero di core della CPU da usare:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="553"/>
        <source>md5</source>
        <translation>md5</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="563"/>
        <source>Options:</source>
        <translation>Opzioni:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="608"/>
        <source>Edit Exclusion File</source>
        <translation>Edita il file delle esclusioni</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="643"/>
        <source>Pictures</source>
        <translation>Immagini</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="650"/>
        <source>Videos</source>
        <translation>Video</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="657"/>
        <source>All of the above</source>
        <translation>Tutti gli elementi sopra</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="664"/>
        <source>exclude network configurations</source>
        <translation>escludi configurazioni di rete</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="667"/>
        <source>Networks</source>
        <translation>Reti</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="674"/>
        <source>Downloads</source>
        <translation>Download</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="681"/>
        <source>Desktop</source>
        <translation>Desktop</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="688"/>
        <source>Documents</source>
        <translation>Documenti</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="695"/>
        <source>Flatpaks</source>
        <translation>Pacchetti Flatpaks</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="702"/>
        <source>Music</source>
        <translation>Musica</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="814"/>
        <source>About this application</source>
        <translation>Informazioni su questa applicazione</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="817"/>
        <source>About...</source>
        <translation>Info...</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="823"/>
        <source>Alt+B</source>
        <translation>Alt+B</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="839"/>
        <source>Next</source>
        <translation>Successivo</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="864"/>
        <source>Display help </source>
        <translation>Visualizza la guida</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="867"/>
        <source>Help</source>
        <translation>Aiuto</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="873"/>
        <source>Alt+H</source>
        <translation>Alt+H</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="886"/>
        <source>Quit application</source>
        <translation>Esci dall&apos;applicazione</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="889"/>
        <source>Cancel</source>
        <translation>Annulla</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="895"/>
        <source>Alt+N</source>
        <translation>Alt+N</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="949"/>
        <source>Back</source>
        <translation>Indietro</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="147"/>
        <source>Select Release Date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fastest, worst compression</source>
        <translation>La compressione più veloce, la peggiore</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>fast, worse compression</source>
        <translation>Compressione veloce, peggiore</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>slow, better compression</source>
        <translation>Compressione lenta, migliore</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="192"/>
        <source>best compromise</source>
        <translation>Miglior compromesso</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="193"/>
        <source>slowest, best compression</source>
        <translation>La compressione più lenta, la migliore</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="222"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>Spazio libero su %1, dove la cartella della snapshot è localizzata: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="225"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space by removing previous snapshots and saved copies: %1 snapshots are taking up %2 of disk space.</source>
        <translation>Lo spazio libero dovrebbe essere sufficiente per contenere i dati compressi da / e /home

Se necessario, puoi creare più spazio disponibile rimuovendo istantanee precedenti e copie salvate: %1 istantanea occupa circa %2 di spazio su disco.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="237"/>
        <location filename="../mainwindow.cpp" line="238"/>
        <source>Installing </source>
        <translation>Installazione in corso</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="323"/>
        <source>Please wait.</source>
        <translation>Attendi, per favore</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="325"/>
        <source>Please wait. Calculating used disk space...</source>
        <translation>Prego attendi. Calcolo dello spazio usato sul disco...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="359"/>
        <location filename="../mainwindow.cpp" line="416"/>
        <source>Error</source>
        <translation>Errore</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="360"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>Il file di uscita %1 esiste già. Utilizzare un altro nome file o eliminare il file esistente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="365"/>
        <source>Settings</source>
        <translation>Impostazioni</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="371"/>
        <source>Snapshot will use the following settings:</source>
        <translation>Istantanea userà le seguenti impostazioni:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="373"/>
        <source>- Snapshot directory:</source>
        <translation>- Snapshot directory:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="374"/>
        <source>- Kernel to be used:</source>
        <translation>- Kernel da utilizzare:</translation>
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
        <translation>Il kernel attuale non supporta l&apos;algoritmo di compressione selezionato, prego modifica il file di configurazione e scegli un algoritmo differente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="441"/>
        <source>Final chance</source>
        <translation>Ultima occasione</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="443"/>
        <source>Snapshot now has all the information it needs to create an ISO from your running system.</source>
        <translation>Snapshot ha ora tutte le informazioni necessarie per creare una ISO del tuo attuale sistema in uso.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="444"/>
        <source>It will take some time to finish, depending on the size of the installed system and the capacity of your computer.</source>
        <translation>Ci vorrà parecchio tempo per finire, in funzione della dimensione del sistema installato e della velocità del tuo computer</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="446"/>
        <source>OK to start?</source>
        <translation>Ok ad iniziare?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="450"/>
        <source>Shutdown computer when done.</source>
        <translation>Spegni il computer quando terminato.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="483"/>
        <source>Output</source>
        <translation>Output</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="499"/>
        <source>Close</source>
        <translation>Chiudi</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="506"/>
        <source>Edit Boot Menu</source>
        <translation>Modifica il menù di avvio</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="507"/>
        <source>The program will now pause to allow you to edit any files in the work directory. Select Yes to edit the boot menu or select No to bypass this step and continue creating the snapshot.</source>
        <translation>Il programma ora andrà in pausa per permetterti di editare alcuni files nella directory di lavoro. Seleziona Yes per editare il menù del boot o seleziona No per bypassare questa fase e continuare a creare la snapshot.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="603"/>
        <source>About %1</source>
        <translation>Circa %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="605"/>
        <source>Version: </source>
        <translation>Versione: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="606"/>
        <source>Program for creating a live-CD from the running system for MX Linux</source>
        <translation>Programma di MX Linux per creare un CD live dal sistema in esecuzione</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="608"/>
        <source>Copyright (c) MX Linux</source>
        <translation>Copyright (c) MX Linux</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="610"/>
        <source>%1 License</source>
        <translation>%1 Licenza</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="624"/>
        <source>%1 Help</source>
        <translation>%1 Aiuto</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="629"/>
        <source>Select Snapshot Directory</source>
        <translation>Seleziona la directory di Snapshot</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Confirmation</source>
        <translation>Conferma</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="650"/>
        <source>Are you sure you want to quit the application?</source>
        <translation>Sei sicuro di voler chiudere l&apos;applicazione?</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="71"/>
        <source>Tool used for creating a live-CD from the running system</source>
        <translation>Strumento usato per creare un CD-live dal sistema in uso</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="75"/>
        <source>Use CLI only</source>
        <translation>Usa solo la CLI</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="79"/>
        <source>Number of CPU cores to be used.</source>
        <translation>Numero di core della CPU che devono essere usati</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="80"/>
        <source>Output directory</source>
        <translation>Directory di destinazione</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="81"/>
        <source>Output filename</source>
        <translation>Nome del file di destinazione</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="83"/>
        <source>Name a different kernel to use other than the default running kernel, use format returned by &apos;uname -r&apos;</source>
        <translation>Scegli un nome di un kernel diverso da utilizzare al posto del kernel in esecuzione predefinito, usa il formato mostrato da &apos;uname -r&apos;</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="85"/>
        <source>Or the full path: %1</source>
        <translation>O il percorso completo:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="88"/>
        <source>Compression level options.</source>
        <translation>Opzioni del livello di compressione.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="89"/>
        <source>Use quotes: &quot;-Xcompression-level &lt;level&gt;&quot;, or &quot;-Xalgorithm &lt;algorithm&gt;&quot;, or &quot;-Xhc&quot;, see mksquashfs man page</source>
        <translation>Usa le espressioni: &quot;-Xcompression-level &lt;level&gt;&quot;, o &quot;-Xalgorithm &lt;algorithm&gt;&quot;, o &quot;-Xhc&quot;, vedi la pagina di mksquashfs man</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="93"/>
        <source>Create a monthly snapshot, add &apos;Month&apos; name in the ISO name, skip used space calculation</source>
        <translation>Crea un&apos;istantanea mensile, aggiungi il nome &quot;Mese&quot; nel nome ISO, salta il calcolo dello spazio utilizzato</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="94"/>
        <source>This option sets reset-accounts and compression to defaults, arguments changing those items will be ignored</source>
        <translation>Questa opzione imposta il reset degli account e la compressione ai valori predefiniti, gli argomenti che cambiano questi elementi verranno ignorati</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="97"/>
        <source>Don&apos;t calculate checksums for resulting ISO file</source>
        <translation>Non calcolare i checksum per il file ISO risultante</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="98"/>
        <source>Skip calculating free space to see if the resulting ISO will fit</source>
        <translation>Salta il calcolo dello spazio libero per vedere se l&apos;ISO risultante si adatterà</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="99"/>
        <source>Option to fix issue with calculating checksums on preempt_rt kernels</source>
        <translation>Opzione per risolvere il problema con il calcolo delle somme sui kernel preempt_rt</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="100"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Resettaggio degli account (per distribuire ad altri)</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="101"/>
        <source>Calculate checksums for resulting ISO file</source>
        <translation>Calcola i checksum per il file ISO risultante</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="103"/>
        <source>Throttle the I/O input rate by the given percentage. This can be used to reduce the I/O and CPU consumption of Mksquashfs.</source>
        <translation>Limita la velocità di ingresso dell&apos;I/O della percentuale indicata. Questo può essere usato per ridurre il consumo di I/O e di CPU di Mksquashfs.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="106"/>
        <source>Work directory</source>
        <translation>Directory di lavoro</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="108"/>
        <source>Exclude main folders, valid choices: </source>
        <translation>Escludi la cartella principale, scelte valide:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="110"/>
        <source>Use the option one time for each item you want to exclude</source>
        <translation>Usa l&apos;opzione una volta per ciascun elemento che vuoi escludere</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="113"/>
        <source>Compression format, valid choices: </source>
        <translation>Formato di compressione, scelte valide:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="115"/>
        <source>Shutdown computer when done.</source>
        <translation>Spegni il computer quando terminato.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="144"/>
        <source>You seem to be logged in as root, please log out and log in as normal user to use this program.</source>
        <translation>Sembra che tu sia loggato come root, fai il log out e poi il log in come utente normale per usare questo programma.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="169"/>
        <source>version:</source>
        <translation>versione:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="163"/>
        <source>You must run this program with sudo or pkexec.</source>
        <translation>Devi eseguire questo programma con sudo o pkexec.</translation>
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
        <translation>Errore</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="216"/>
        <location filename="../settings.cpp" line="248"/>
        <source>Current kernel doesn&apos;t support Squashfs, cannot continue.</source>
        <translation>Il kernel attuale non supporta Squashfs, non si può continuare.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="85"/>
        <source>Could not create working directory. </source>
        <translation>Impossibile creare una directory attiva.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="109"/>
        <source>Could not create temp directory. </source>
        <translation>Impossibile creare una directory temporanea.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="233"/>
        <source>Could not find a usable kernel</source>
        <translation>Impossibile trovare un kernel utilizzabile</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="357"/>
        <source>Used space on / (root): </source>
        <translation>Spazio utilizzato su / (root):</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="360"/>
        <source>estimated</source>
        <translation>stimato</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="369"/>
        <source>Used space on /home: </source>
        <translation>Spazio utilizzato su /home:</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="463"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>Spazio libero su %1, dove la cartella della snapshot è localizzata: </translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="467"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space
      by removing previous snapshots and saved copies:
      %1 snapshots are taking up %2 of disk space.
</source>
        <translation>Lo spazio libero dovrebbe essere sufficiente per contenere i dati compressi da / e da /home

Se necessario, puoi creare più spazio disponibile
rimuovendo precedenti snapshots e copie salvate:
%1 snapshot occupa circa %2 di spazio del disco.
</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="485"/>
        <source>Desktop</source>
        <translation>Desktop</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="486"/>
        <source>Documents</source>
        <translation>Documenti</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="487"/>
        <source>Downloads</source>
        <translation>Download</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="488"/>
        <source>Flatpaks</source>
        <translation>Pacchetti Flatpaks</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="489"/>
        <source>Music</source>
        <translation>Musica</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="490"/>
        <source>Networks</source>
        <translation>Reti</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="491"/>
        <source>Pictures</source>
        <translation>Immagini</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="493"/>
        <source>Videos</source>
        <translation>Video</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="743"/>
        <location filename="../settings.cpp" line="842"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>Il file di uscita %1 esiste già. Utilizzare un altro nome file o eliminare il file esistente.</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="70"/>
        <source>License</source>
        <translation>Licenza</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="71"/>
        <location filename="../about.cpp" line="81"/>
        <source>Changelog</source>
        <translation>Registro delle modifiche</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="72"/>
        <source>Cancel</source>
        <translation>Annulla</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="94"/>
        <source>&amp;Close</source>
        <translation>&amp;Chiudi</translation>
    </message>
</context>
<context>
    <name>Work</name>
    <message>
        <location filename="../work.cpp" line="82"/>
        <source>Cleaning...</source>
        <translation>Pulizia in corso...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="93"/>
        <location filename="../work.cpp" line="283"/>
        <source>Done</source>
        <translation>Fatto</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="105"/>
        <source>Interrupted or failed to complete</source>
        <translation>Completamento interrotto o fallito</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="138"/>
        <location filename="../work.cpp" line="241"/>
        <location filename="../work.cpp" line="263"/>
        <location filename="../work.cpp" line="301"/>
        <location filename="../work.cpp" line="439"/>
        <source>Error</source>
        <translation>Errore</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="139"/>
        <source>There&apos;s not enough free space on your target disk, you need at least %1</source>
        <translation>Non c&apos;è sufficiente spazio libero sul tuo disco di destinazione, hai bisogno di almeno</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="142"/>
        <source>You have %1 free space on %2</source>
        <translation>Hai spazio libero su</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="145"/>
        <source>If you are sure you have enough free space rerun the program with -o/--override-size option</source>
        <translation>Se sei sicuro di avere abbastanza spazio libero, esegui nuovamente il programma con l&apos;opzione -o/--override-size</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="170"/>
        <source>Copying the new-iso filesystem...</source>
        <translation>Copia del filesystem della nuova-iso ...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="182"/>
        <source>Could not create temp directory. </source>
        <translation>Impossibile creare una directory temporanea.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="238"/>
        <source>Squashing filesystem...</source>
        <translation>Compressione del filesystem...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="242"/>
        <source>Could not create linuxfs file, please check /var/log/%1.log</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="260"/>
        <source>Creating CD/DVD image file...</source>
        <translation>Creazione del file immagine CD/DVD in corso...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="264"/>
        <source>Could not create ISO file, please check whether you have enough space on the destination partition.</source>
        <translation>Impossibile creare il file ISO, verifica di avere spazio a sufficienza sulla partizione di destinazione.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="270"/>
        <source>Making hybrid iso</source>
        <translation>Creazione di un&apos;ISO ibrida in corso</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="288"/>
        <source>Success</source>
        <translation>Operazione riuscita</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="289"/>
        <source>MX Snapshot completed successfully!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../work.cpp" line="290"/>
        <source>Snapshot took %1 to finish.</source>
        <translation>L&apos;istantanea ha richeisto l&apos;1% per terminare</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="291"/>
        <source>Thanks for using MX Snapshot, run MX Live USB Maker next!</source>
        <translation>Grazie per aver usato MX Snapshot, prova anche MX Live Maker!</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="298"/>
        <source>Installing </source>
        <translation>Installazione in corso</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="301"/>
        <source>Could not install </source>
        <translation>Non è stato possibile installare</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="311"/>
        <source>Calculating checksum...</source>
        <translation>Calcolo checksum...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="349"/>
        <source>Building new initrd...</source>
        <translation>Realizzazione del nuovo initrd...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="440"/>
        <source>Could not create working directory. </source>
        <translation>Impossibile creare una directory attiva.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="578"/>
        <source>Calculating total size of excluded files...</source>
        <translation>Calcolo della dimensione totale dei file esclusi...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="588"/>
        <source>Calculating size of root...</source>
        <translation>Calcolo la dimensione di root...</translation>
    </message>
</context>
</TS>
