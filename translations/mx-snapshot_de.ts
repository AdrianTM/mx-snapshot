<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de">
<context>
    <name>Batchprocessing</name>
    <message>
        <location filename="../batchprocessing.cpp" line="44"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="45"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>Der installierte Kernel unterstützt den ausgewählten Kompressionsalgorithmus nicht; bitte die Konfigurationdatei bearbeiten und einen anderen Algorithmus auswählen.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="71"/>
        <source>The program will pause the build and open the boot menu in your text editor.</source>
        <translation>Das Programm pausiert, während das Bootmenü im Texteditor geöffnet wird.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="101"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>Dieser Computer verwendet eine NVIDIA Grafikkarte. Möchten Sie die erstellte ISO auf demselben Computer oder auf einem anderen Computer mit einer NVIDIA-Grafikkarte verwenden?</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="111"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>Hinweis: Wenn Sie die erstellte ISO auf einem Computer ohne NVIDIA-Grafikkarte verwenden, müssen Sie gegebenenfalls &quot;xorg=nvidia&quot; aus den Boot-Optionen entfernen.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="114"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>Hinweis: Wenn Sie die erstellte ISO auf einem Computer mit einer NVIDIA-Grafikkarte verwenden, müssen Sie gegebenenfalls &quot;xorg=nvidia&quot; zu den Boot-Optionen hinzufügen. </translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.ui" line="14"/>
        <location filename="../mainwindow.cpp" line="183"/>
        <location filename="../mainwindow.cpp" line="517"/>
        <source>MX Snapshot</source>
        <translation>MX-Schnappschuss</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="33"/>
        <source>Optional customization</source>
        <translation>Optionale Anpassungen</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="100"/>
        <source>Release date:</source>
        <translation>Datum der Freigabe:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="107"/>
        <source>Release codename:</source>
        <translation>Codename der Freigabe:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="86"/>
        <source>Live kernel:</source>
        <translation>Live-Kernel:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="79"/>
        <source>Boot options:</source>
        <translation>Boot-Optionen:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="65"/>
        <source>Release version:</source>
        <translation>Release Version:</translation>
    </message>
    <message>
        <source>Change live kernel</source>
        <translation type="vanished">Live-Kernel wechseln</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="93"/>
        <source>Project name:</source>
        <translation>Projektname:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="133"/>
        <source>Current date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot is a utility that creates a bootable image (ISO) of your working system that you can use for storage or distribution. You can continue working with undemanding applications while it is running.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Mit &quot;snapshot&quot; können Sie sehr einfach ein bootfähiges Image (.ISO) Ihres modifizierten Arbeitssystems erzeugen für USB-stick oder CD-ROM. Das dient z.B. als Datensicherung oder kann auch weiter verteilt werden (z.B. modifizierte distro mit account reset). Während die snapshot-Erstellung im Hintergrund läuft, können Sie mit den davon nicht abhängigen Programmen weiter arbeiten, wenn Sie möchten.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="182"/>
        <source>Used space on / (root) and /home partitions:</source>
        <translation>Belegter Platz für / (root) und /home Partitionen:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="192"/>
        <source>Location and ISO name</source>
        <translation>Ort und Name der ISO-Datei</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="219"/>
        <source>Snapshot location:</source>
        <translation>Speicherort des Schnappschusses:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="226"/>
        <source>Select a different snapshot directory</source>
        <translation>Anderes Verzeichnis für den Schnappschuss wählen</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="236"/>
        <location filename="../mainwindow.cpp" line="372"/>
        <source>Snapshot name:</source>
        <translation>Name des Schnappschusses:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="355"/>
        <source>You can also exclude certain directories by ticking the common choices below, or by clicking on the button to directly edit /etc/mx-snapshot-exclude.list.</source>
        <translation>Sie können bestimmte Verzeichnisse ausschließen, indem Sie diese in der vorgegebenen Auswahl ankreuzen, oder durch direktes Bearbeiten der Datei /etc/mx-snapshot-exclude.list, die sich mit einem Klick auf die Schaltfläche öffnet.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="412"/>
        <source>exclude network configurations</source>
        <translation> Netzwerkkonfigurationen ausschließen</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="415"/>
        <source>Networks</source>
        <translation>Netzwerke</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="422"/>
        <source>All of the above</source>
        <translation>Alles hier genannte</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="429"/>
        <source>Pictures</source>
        <translation>Bilder</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="446"/>
        <source>Desktop</source>
        <translation>Benutzeroberfläche</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="463"/>
        <source>Music</source>
        <translation>Musik</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="470"/>
        <source>Downloads</source>
        <translation>Downloads</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="477"/>
        <source>Videos</source>
        <translation>Videos</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="484"/>
        <source>Documents</source>
        <translation>Dokumente</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="522"/>
        <source>Type of snapshot:</source>
        <translation>Art des Schnappschusses</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="529"/>
        <source>Preserving accounts (for personal backup)</source>
        <translation>Benutzerkonten beibehalten (z.B. wegen persönlichem Backup)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="539"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option will reset &amp;quot;demo&amp;quot; and &amp;quot;root&amp;quot; passwords to the MX Linux defaults and will not copy any personal accounts created.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Diese Option setzt das Passwort von Benutzern &quot;demo&quot; und &quot;root&quot; auf die Vorgabe von MX Linux zurück (nämlich &quot;demo&quot; und &quot;root&quot;) aber kopiert keine anderen persönlich erstellten Benutzerkonten. Passwortänderung ohne Kenntnis des alten Passwortes ist auch jederzeit im boot-Menü möglich: F4 &quot;private&quot;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="542"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Account zurücksetzen (z.B. wegen Veröffentlichung als Distro)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="571"/>
        <source>Edit Exclusion File</source>
        <translation>Ausschluss-Datei bearbeiten</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="609"/>
        <source>sha512</source>
        <translation>sha512</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="632"/>
        <source>Throttle the I/O input rate by the given percentage.</source>
        <translation>Drosselung der I/O-Eingangsrate um den angegebenen Prozentsatz.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="638"/>
        <source>I/O throttle:</source>
        <translation>I/O Drosselung:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="645"/>
        <source>Calculate checksums:</source>
        <translation>Prüfsummen berechnen:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="652"/>
        <source>ISO compression scheme:</source>
        <translation>ISO Kompressionschema :</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="659"/>
        <source>Number of CPU cores to use:</source>
        <translation>Anzahl der zu verwendenden CPU-Kerne:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="666"/>
        <source>md5</source>
        <translation>md5</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="676"/>
        <source>Options:</source>
        <translation>Auswahlmöglichkeiten:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="823"/>
        <source>About this application</source>
        <translation>Über dieses Programm</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="826"/>
        <source>About...</source>
        <translation>Über...</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="832"/>
        <source>Alt+B</source>
        <translation>Alt+B</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="848"/>
        <source>Quit application</source>
        <translation>Anwendung beenden</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="851"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="857"/>
        <source>Alt+N</source>
        <translation>Alt+N</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="889"/>
        <source>Next</source>
        <translation>Weiter</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="936"/>
        <source>Back</source>
        <translation>Zurück</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="952"/>
        <source>Display help </source>
        <translation>Hilfe anzeigen</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="955"/>
        <source>Help</source>
        <translation>Hilfe</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="961"/>
        <source>Alt+H</source>
        <translation>Alt+H</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="145"/>
        <source>Select Release Date</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="189"/>
        <source>fastest, worst compression</source>
        <translation>schnellste, schlechteste Komprimierung</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="189"/>
        <source>fast, worse compression</source>
        <translation>schnelle, schlechte Komprimierung</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="190"/>
        <source>slow, better compression</source>
        <translation>langsame, bessere Komprimierung</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="190"/>
        <source>best compromise</source>
        <translation>bester Kompromiss</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>slowest, best compression</source>
        <translation>langsamste, beste Komprimierung</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="220"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>Freier Platz auf %1, wo das Schnappschuss-Verzeichnis liegt:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="223"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space by removing previous snapshots and saved copies: %1 snapshots are taking up %2 of disk space.</source>
        <translation>Der freie Platz sollte ausreichen, um die komprimierten Daten von / und /home aufzunehmen

      Bei Bedarf können Sie mehr freien Platz gewinnen, indem Sie
gespeicherte Kopien von früheren Schnappschüssen löschen:
%1 Schnappschuss verbraucht %2 Festplattenplatz.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="235"/>
        <location filename="../mainwindow.cpp" line="236"/>
        <source>Installing </source>
        <translation>Installiere</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="321"/>
        <source>Please wait.</source>
        <translation>Bitte warten.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="323"/>
        <source>Please wait. Calculating used disk space...</source>
        <translation>Bitte warten. Berechne benötigten Platz auf Festplatte...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="357"/>
        <location filename="../mainwindow.cpp" line="414"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="358"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>Die Ausgabedatei %1 existiert bereits. Bitte verwenden Sie einen anderen Dateinamen, oder löschen Sie die existierende Datei.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="363"/>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="369"/>
        <source>Snapshot will use the following settings:</source>
        <translation>Der Schnappschuss wird folgende Einstellungen verwenden:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="371"/>
        <source>- Snapshot directory:</source>
        <translation>- Schnappschuss-Verzeichnis:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="372"/>
        <source>- Kernel to be used:</source>
        <translation>- Verwendeter Kernel:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="394"/>
        <location filename="../mainwindow.cpp" line="403"/>
        <source>NVIDIA Detected</source>
        <translation>NVIDIA-Grafikkarte wurde erkannt</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="395"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>Dieser Computer verwendet eine NVIDIA Grafikkarte. Möchten Sie die erstellte ISO auf demselben Computer oder auf einem anderen Computer mit einer NVIDIA-Grafikkarte verwenden?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="399"/>
        <source>NVIDIA Selected</source>
        <translation>NVIDIA-Grafikkarte ausgewählt</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="400"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>Hinweis: Wenn Sie die erstellte ISO auf einem Computer ohne NVIDIA-Grafikkarte verwenden, müssen Sie gegebenfalls &quot;xorg=nvidia&quot; aus den Boot-Optionen entfernen.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="404"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>Hinweis: Wenn Sie die erstellte ISO auf einem Computer mit einer NVIDIA-Grafikkarte verwenden, müssen Sie gegebenenfalls &quot;xorg=nvidia&quot; zu den Boot-Optionen hinzufügen. </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="415"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>Der installierte Kernel unterstützt den ausgewählten Kompressionsalgorithmus nicht; bitte die Konfigurationdatei bearbeiten und einen anderen Algorithmus auswählen.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="439"/>
        <source>Final chance</source>
        <translation>Letzte Chance</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="441"/>
        <source>Snapshot now has all the information it needs to create an ISO from your running system.</source>
        <translation>Das Schnappschuss-Programm hat jetzt alle nötigen Informationen, um ein ISO Ihres laufenden Systems zu erstellen.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="442"/>
        <source>It will take some time to finish, depending on the size of the installed system and the capacity of your computer.</source>
        <translation>Abhängig von der Größe Ihres installierten Systems und der Leistungsfähigkeit des Computers wird es einige Zeit dauern, diesen Vorgang abzuschließen.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="444"/>
        <source>OK to start?</source>
        <translation>Alles klar zum Loslegen?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="448"/>
        <source>Shutdown computer when done.</source>
        <translation>Den Computer ausschalten, wenn die Arbeit erledigt ist.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="480"/>
        <source>Output</source>
        <translation>Ausgabe</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="496"/>
        <source>Close</source>
        <translation>Schließen</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="503"/>
        <source>Edit Boot Menu</source>
        <translation>Boot-Menü bearbeiten</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="504"/>
        <source>The program will now pause to allow you to edit any files in the work directory. Select Yes to edit the boot menu or select No to bypass this step and continue creating the snapshot.</source>
        <translation>Das Programm wartet nun und ermöglicht, alle Dateien im Arbeitsverzeichnis zu bearbeiten.&quot;Yes&quot; wählen, um das Boot-Menü zu bearbeiten, oder &quot;No&quot;, um mit dem Erstellen des Schnappschusses fortzufahren.</translation>
    </message>
    <message>
        <source>Select kernel</source>
        <translation type="vanished">Kernel auswählen</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="593"/>
        <source>About %1</source>
        <translation>Über %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="595"/>
        <source>Version: </source>
        <translation>Version:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="596"/>
        <source>Program for creating a live-CD from the running system for MX Linux</source>
        <translation>Programm, um eine Live-CD Ihres mit MX Linux laufenden Systems zu erstellen.
Es wird eine .ISO-Datei erstellt, bootfähig auch von USB-Stick. Es ist typischerweise unnötig, das laufende System vorher extra zu remastern, was ca. 15 Minuten dauern würde. Alles, was im RAM zwischengespeichert ist, landet in der ISO-Datei, abzüglich der Ausschlüsse.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="598"/>
        <source>Copyright (c) MX Linux</source>
        <translation>Copyright (c) MX Linux</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="600"/>
        <source>%1 License</source>
        <translation>%1 Lizenz</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="614"/>
        <source>%1 Help</source>
        <translation>%1 Hilfe</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="619"/>
        <source>Select Snapshot Directory</source>
        <translation>Wähle Schnappschuss-Verzeichnis</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="640"/>
        <source>Confirmation</source>
        <translation>Bestätigung</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="640"/>
        <source>Are you sure you want to quit the application?</source>
        <translation>Sicherheitsabfrage: Programm wirklich beenden? </translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="71"/>
        <source>Tool used for creating a live-CD from the running system</source>
        <translation>Werkzeug zur Erzeugung einer Live-CD aus dem laufenden System heraus</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="75"/>
        <source>Use CLI only</source>
        <translation>Nur CLI verwenden</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="79"/>
        <source>Number of CPU cores to be used.</source>
        <translation>Anzahl der verwendeten CPU-Kerne:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="80"/>
        <source>Output directory</source>
        <translation>Zielverzeichnis</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="81"/>
        <source>Output filename</source>
        <translation>Name der Ausgabedatei</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="83"/>
        <source>Name a different kernel to use other than the default running kernel, use format returned by &apos;uname -r&apos;</source>
        <translation>Geben Sie bitte einen anderen als den aktiven Kernel des laufenden Systems an und benutzen Sie dafür das gleiche Format, das vom Befehl &apos;uname -r&apos; ausgegeben wird.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="85"/>
        <source>Or the full path: %1</source>
        <translation>Oder vollständige Pfadangabe: %1</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="88"/>
        <source>Compression level options.</source>
        <translation>Optionen für die Kompressionsstufe.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="89"/>
        <source>Use quotes: &quot;-Xcompression-level &lt;level&gt;&quot;, or &quot;-Xalgorithm &lt;algorithm&gt;&quot;, or &quot;-Xhc&quot;, see mksquashfs man page</source>
        <translation>Verwenden Sie Anführungszeichen: &quot;-Xcompression-level &quot;, oder &quot;-Xalgorithm &quot;, oder &quot;-Xhc&quot;, siehe mksquashfs-Benutzerhandbuch</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="93"/>
        <source>Create a monthly snapshot, add &apos;Month&apos; name in the ISO name, skip used space calculation</source>
        <translation>Erzeuge einen Monatsschnappschuß, füge den aktuellen &apos;Monat&apos; in den Dateinamen des ISOs ein und übergehe die Größenberechnung.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="94"/>
        <source>This option sets reset-accounts and compression to defaults, arguments changing those items will be ignored</source>
        <translation>Diese Option setzt die Einstellung zum Zurücksetzen der Nutzerkonten und der Kompression auf Standardkonfiguration. Argumente, die diese Einstellungen ändern, werden ignoriert.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="97"/>
        <source>Don&apos;t calculate checksums for resulting ISO file</source>
        <translation>Keine Prüfsumme für die erzeugte ISO Datei generieren.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="98"/>
        <source>Skip calculating free space to see if the resulting ISO will fit</source>
        <translation>Berechnen des freien Speicherplatzes abbrechen, um zu sehen, ob die resultierende ISO passt.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="99"/>
        <source>Option to fix issue with calculating checksums on preempt_rt kernels</source>
        <translation>Option zum Beheben von Problemen der Prüfsummenberechung bei einem Kernel des Typs &quot;preempt_rt&quot;.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="100"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Account zurücksetzen (z.B. wegen Veröffentlichung als Distro)</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="101"/>
        <source>Calculate checksums for resulting ISO file</source>
        <translation>Berechnen der Prüfsumme für die erzeugte ISO Datei.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="103"/>
        <source>Throttle the I/O input rate by the given percentage. This can be used to reduce the I/O and CPU consumption of Mksquashfs.</source>
        <translation>Drosselt die I/O-Eingangsrate um den angegebenen Prozentsatz. Dies kann verwendet werden, um den I/O- und CPU-Verbrauch von Mksquashfs zu reduzieren.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="106"/>
        <source>Work directory</source>
        <translation>Arbeitsverzeichnis</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="108"/>
        <source>Exclude main folders, valid choices: </source>
        <translation>Wahlmöglichkeiten für den Ausschluß von Hauptverzeichnissen:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="110"/>
        <source>Use the option one time for each item you want to exclude</source>
        <translation>Diese Option einmal für jeden Eintrag, der ausgeschlossen werden soll, verwenden.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="113"/>
        <source>Compression format, valid choices: </source>
        <translation>Wahlmöglichkeiten für die Art der Kompression:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="115"/>
        <source>Shutdown computer when done.</source>
        <translation>Den Computer ausschalten, wenn die Arbeit erledigt ist.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="144"/>
        <source>You seem to be logged in as root, please log out and log in as normal user to use this program.</source>
        <translation>Sie sind als Administrator am System angemeldet. Bitte melden Sie sich ab und dann als normaler Benutzer wieder an, um dieses Programm zu verwenden.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="168"/>
        <source>version:</source>
        <translation>Version:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="163"/>
        <source>You must run this program with sudo or pkexec.</source>
        <translation>Sie müssen dieses Programm mit sudo oder pkexec ausführen.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="139"/>
        <source>MX Snapshot</source>
        <translation>MX-Schnappschuss</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="148"/>
        <location filename="../main.cpp" line="219"/>
        <location filename="../settings.cpp" line="240"/>
        <location filename="../settings.cpp" line="255"/>
        <location filename="../settings.cpp" line="732"/>
        <location filename="../settings.cpp" line="831"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="216"/>
        <location filename="../settings.cpp" line="249"/>
        <source>Current kernel doesn&apos;t support Squashfs, cannot continue.</source>
        <translation>Der installierte Kernel unterstützt Squashfs nicht; Abbruch.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="85"/>
        <source>Could not create working directory. </source>
        <translation>Konnte kein Arbeitsverzeichnis anlegen.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="109"/>
        <source>Could not create temp directory. </source>
        <translation>Konnte kein Temporärverzeichnis anlegen.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="234"/>
        <source>Could not find a usable kernel</source>
        <translation>Kein verwendbarer Kernel vorhanden.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="358"/>
        <source>Used space on / (root): </source>
        <translation>Benötigter Platz für das Wurzelverzeichnis &quot;/&quot; (root):</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="361"/>
        <source>estimated</source>
        <translation>geschätzt</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="370"/>
        <source>Used space on /home: </source>
        <translation>Benötigter Platz für /home:</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="458"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>Freier Platz auf %1, wo das Schnappschuss-Verzeichnis liegt:</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="462"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space
      by removing previous snapshots and saved copies:
      %1 snapshots are taking up %2 of disk space.
</source>
        <translation>Der freie Platz sollte für die komprimierten Daten von / und /home ausreichen.

Bei Bedarf können Sie Platz gewinnen, indem Sie
gespeicherte Kopien von früheren Schnappschüssen löschen:
%1 Schnappschuss verbraucht %2 Festplattenplatz.
</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="480"/>
        <source>Desktop</source>
        <translation>Benutzeroberfläche</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="481"/>
        <source>Documents</source>
        <translation>Dokumente</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="482"/>
        <source>Downloads</source>
        <translation>Downloads</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="483"/>
        <source>Music</source>
        <translation>Musik</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="484"/>
        <source>Networks</source>
        <translation>Netzwerke</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="485"/>
        <source>Pictures</source>
        <translation>Bilder</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="487"/>
        <source>Videos</source>
        <translation>Videos</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="725"/>
        <location filename="../settings.cpp" line="824"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>Die Ausgabedatei %1 existiert bereits. Bitte verwenden Sie einen anderen Dateinamen, oder löschen Sie die existierende Datei.</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="70"/>
        <source>License</source>
        <translation>Lizenz</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="71"/>
        <location filename="../about.cpp" line="81"/>
        <source>Changelog</source>
        <translation>Änderungsprotokoll</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="72"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="94"/>
        <source>&amp;Close</source>
        <translation>&amp;Schließen</translation>
    </message>
</context>
<context>
    <name>Work</name>
    <message>
        <location filename="../work.cpp" line="82"/>
        <source>Cleaning...</source>
        <translation>Bereinigung läuft...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="93"/>
        <location filename="../work.cpp" line="283"/>
        <source>Done</source>
        <translation>Fertig</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="105"/>
        <source>Interrupted or failed to complete</source>
        <translation>Fertigstellung fehlgeschlagen oder Ausführung unterbrochen.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="138"/>
        <location filename="../work.cpp" line="241"/>
        <location filename="../work.cpp" line="263"/>
        <location filename="../work.cpp" line="301"/>
        <location filename="../work.cpp" line="414"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="139"/>
        <source>There&apos;s not enough free space on your target disk, you need at least %1</source>
        <translation>Der Speicherplatz auf dem Ziellaufwerk reicht nicht aus. Es werden mindestens %1 benötigt.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="142"/>
        <source>You have %1 free space on %2</source>
        <translation>Auf %2 ist %1 Speicherplatz frei.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="145"/>
        <source>If you are sure you have enough free space rerun the program with -o/--override-size option</source>
        <translation>Wenn Sie sicher sind, genügend freien Speicherplatz zu haben, starten Sie das Programm erneut mit der Option -o/--override-size</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="170"/>
        <source>Copying the new-iso filesystem...</source>
        <translation>Kopiere Dateisystem für neue ISO...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="182"/>
        <source>Could not create temp directory. </source>
        <translation>Konnte kein Temporärverzeichnis anlegen.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="238"/>
        <source>Squashing filesystem...</source>
        <translation>Komprimiere Dateisystem...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="242"/>
        <source>Could not create linuxfs file, please check /var/log/%1.log</source>
        <translation>Die Datei linuxfs konnte nicht erstellt werden, bitte /var/log/%1.log überprüfen</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="260"/>
        <source>Creating CD/DVD image file...</source>
        <translation>Erstelle Image-Datei der CD/DVD...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="264"/>
        <source>Could not create ISO file, please check whether you have enough space on the destination partition.</source>
        <translation>Die .ISO-Datei konnte nicht erstellt werden, überprüfen Sie bitte, ob genug Speicherplatz frei ist.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="270"/>
        <source>Making hybrid iso</source>
        <translation>Hybrid-ISO erstellen</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="288"/>
        <source>Success</source>
        <translation>Erfolg</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="289"/>
        <source>MX Snapshot completed successfully!</source>
        <translation>MX-Schnappschuss erfolgreich abgeschlossen!</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="290"/>
        <source>Snapshot took %1 to finish.</source>
        <translation>Der Schnappschuss benötigte %1 .</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="291"/>
        <source>Thanks for using MX Snapshot, run MX Live USB Maker next!</source>
        <translation>Danke, daẞ Sie MX Snapshot benutzt haben. Jetzt geht&apos;s zum MX Live USB Maker!</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="298"/>
        <source>Installing </source>
        <translation>Installiere</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="301"/>
        <source>Could not install </source>
        <translation>Konnte nicht installieren</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="311"/>
        <source>Calculating checksum...</source>
        <translation>Generiere Prüfsumme...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="349"/>
        <source>Building new initrd...</source>
        <translation>Neue initrd Datei wird erstellt ....</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="415"/>
        <source>Could not create working directory. </source>
        <translation>Konnte kein Arbeitsverzeichnis anlegen.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="545"/>
        <source>Calculating total size of excluded files...</source>
        <translation>Berechne die Gesamtgröße der ausgeschlossenen Dateien ...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="555"/>
        <source>Calculating size of root...</source>
        <translation>Berechne die Größe von root ...</translation>
    </message>
</context>
</TS>
