<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="pt_BR">
<context>
    <name>Batchprocessing</name>
    <message>
        <location filename="../batchprocessing.cpp" line="44"/>
        <source>Error</source>
        <translation>Ocorreu um Erro</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="45"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>O núcleo (kernel) atual não possui suporte ao algoritmo de compactação/compressão que foi selecionado. Por favor, edite o arquivo de configurações e selecione um algoritmo diferente.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="71"/>
        <source>The program will pause the build and open the boot menu in your text editor.</source>
        <translation>O programa irá parar a compilação e irá abrir o arquivo de configurações do menu de inicialização no editor de texto.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="101"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>Este computador utiliza uma placa de vídeo da NVIDIA. Você está planejando utilizar a imagem ISO resultante no mesmo computador ou em um outro computador que tem uma placa de vídeo da NVIDIA?</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="111"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>Observação: Se você utilizar a imagem ISO resultante em um computador que não tem uma placa de vídeo da NVIDIA, provavelmente será necessário remover o parâmetro ‘xorg=nvidia’ das opções de inicialização.</translation>
    </message>
    <message>
        <location filename="../batchprocessing.cpp" line="114"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>Observação: Se você utilizar a imagem ISO resultante em um computador que tem uma placa de vídeo da NVIDIA, pode ser necessário adicionar o parâmetro &apos;xorg=nvidia&apos; nas opções de inicialização.</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.ui" line="14"/>
        <location filename="../mainwindow.cpp" line="183"/>
        <location filename="../mainwindow.cpp" line="517"/>
        <source>MX Snapshot</source>
        <translation>Snapshot do MX - Criador de Imagem ISO do Sistema Operacional</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="33"/>
        <source>Optional customization</source>
        <translation>Opções de Personalização</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="65"/>
        <source>Release version:</source>
        <translation>Versão do lançamento:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="79"/>
        <source>Boot options:</source>
        <translation>Opções de inicialização:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="86"/>
        <source>Live kernel:</source>
        <translation>Núcleo ativo:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="93"/>
        <source>Project name:</source>
        <translation>Nome do projeto:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="100"/>
        <source>Release date:</source>
        <translation>Data do lançamento:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="107"/>
        <source>Release codename:</source>
        <translation>Codinome do lançamento:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="133"/>
        <source>Current date</source>
        <translation>Data atual</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Snapshot is a utility that creates a bootable image (ISO) of your working system that you can use for storage or distribution. You can continue working with undemanding applications while it is running.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;O ‘Snapshot’ é um utilitário que permite criar uma imagem ISO inicializável (bootable) do seu sistema operacional em execução. Esta imagem
ISO pode ser utilizada para guardar/copiar o seu sistema operacional ou para distribuir para outras pessoas. Você pode continuar trabalhando
com programas que sejam pouco exigentes enquanto o ‘Snapshot’ estiver em execução.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="182"/>
        <source>Used space on / (root) and /home partitions:</source>
        <translation>O espaço utilizado nas partições / (‘root’ ou raiz) e /home (pasta pessoal) são:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="192"/>
        <source>Location and ISO name</source>
        <translation>Local e o nome da imagem ISO</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="219"/>
        <source>Snapshot location:</source>
        <translation>Local onde será armazenada a ISO:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="226"/>
        <source>Select a different snapshot directory</source>
        <translation>Selecionar Outra Pasta</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="236"/>
        <location filename="../mainwindow.cpp" line="372"/>
        <source>Snapshot name:</source>
        <translation>Nome da imagem ISO:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="355"/>
        <source>You can also exclude certain directories by ticking the common choices below, or by clicking on the button to directly edit /etc/mx-snapshot-exclude.list.</source>
        <translation>Se você quiser pode selecionar as pastas que não serão incluídas na imagem ISO, ou pode clicar no botão ‘Editar o Arquivo de Exclusão’
para editar manualmente o arquivo de configurações ‘/etc/mx-snapshot-exclude.list’.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="412"/>
        <source>exclude network configurations</source>
        <translation>Excluir as configurações de rede</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="415"/>
        <source>Networks</source>
        <translation>Redes</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="422"/>
        <source>All of the above</source>
        <translation>Todos os itens acima</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="429"/>
        <source>Pictures</source>
        <translation>Imagens</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="446"/>
        <source>Desktop</source>
        <translation>Área de Trabalho</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="463"/>
        <source>Music</source>
        <translation>Músicas</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="470"/>
        <source>Downloads</source>
        <translation>Baixados</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="477"/>
        <source>Videos</source>
        <translation>Vídeos</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="484"/>
        <source>Documents</source>
        <translation>Documentos</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="522"/>
        <source>Type of snapshot:</source>
        <translation>Tipo da imagem ISO:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="529"/>
        <source>Preserving accounts (for personal backup)</source>
        <translation>Preservar a(s) conta(s) do(s) usuário(s) e fazer uma cópia de segurança do Sistema Operacional para o uso pessoal</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="539"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option will reset &amp;quot;demo&amp;quot; and &amp;quot;root&amp;quot; passwords to the MX Linux defaults and will not copy any personal accounts created.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Esta opção permite redefinir as senhas dos usuários ‘demo’ e ‘root’ para as senhas padrões do MX Linux e não irá copiar quaisquer contas pessoais que tenham sido criadas.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="542"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Redefinir as contas e as senhas dos usuários ‘demo’ e ‘root’ para distribuir a imagem ISO para outras pessoas</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="571"/>
        <source>Edit Exclusion File</source>
        <translation>Editar o Arquivo de Exclusão</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="609"/>
        <source>sha512</source>
        <translation>sha512</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="632"/>
        <source>Throttle the I/O input rate by the given percentage.</source>
        <translation>Acelerar a taxa de entrada da E/S ou I/O pelo valor da porcentagem fornecida.</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="638"/>
        <source>I/O throttle:</source>
        <translation>Acelerador de E/S:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="645"/>
        <source>Calculate checksums:</source>
        <translation>Calcular a soma de verificação:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="652"/>
        <source>ISO compression scheme:</source>
        <translation>Esquema de compactação/compressão da ISO:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="659"/>
        <source>Number of CPU cores to use:</source>
        <translation>Quantidade de núcleos do processador a serem utilizados:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="666"/>
        <source>md5</source>
        <translation>md5</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="676"/>
        <source>Options:</source>
        <translation>Opções:</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="823"/>
        <source>About this application</source>
        <translation>Sobre este aplicativo</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="826"/>
        <source>About...</source>
        <translation>Sobre...</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="832"/>
        <source>Alt+B</source>
        <translation>Alt+B</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="848"/>
        <source>Quit application</source>
        <translation>Sair do aplicativo</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="851"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="857"/>
        <source>Alt+N</source>
        <translation>Alt+N</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="889"/>
        <source>Next</source>
        <translation>Próximo</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="936"/>
        <source>Back</source>
        <translation>Anterior</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="952"/>
        <source>Display help </source>
        <translation>Exibir ajuda</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="955"/>
        <source>Help</source>
        <translation>Ajuda</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="961"/>
        <source>Alt+H</source>
        <translation>Alt+H</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="145"/>
        <source>Select Release Date</source>
        <translation>Selecionar a data do lançamento</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="189"/>
        <source>fastest, worst compression</source>
        <translation>Mais rápido e com uma compactação pior</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="189"/>
        <source>fast, worse compression</source>
        <translation>Rápido e com uma compactação pior</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="190"/>
        <source>slow, better compression</source>
        <translation>Lento e com uma compactação melhor</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="190"/>
        <source>best compromise</source>
        <translation>A melhor opção</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="191"/>
        <source>slowest, best compression</source>
        <translation>Mais lento e com uma compactação ainda melhor</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="220"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>O espaço livre na %1 onde a pasta da imagem ISO está localizada no caminho possui </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="223"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space by removing previous snapshots and saved copies: %1 snapshots are taking up %2 of disk space.</source>
        <translation>O espaço livre deve ser suficiente para armazenar os dados compactados da / (raiz do sistema operacional ou ‘root’) e da /home (pasta
pessoal). Se for necessário, você pode liberar mais espaço excluindo as imagens ISOs antigas que foram criadas pelo programa ‘Snapshot’.
As %1 imagens ISOs estão ocupando %2 de espaço em disco.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="235"/>
        <location filename="../mainwindow.cpp" line="236"/>
        <source>Installing </source>
        <translation>Instalando</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="321"/>
        <source>Please wait.</source>
        <translation>Por favor, aguarde.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="323"/>
        <source>Please wait. Calculating used disk space...</source>
        <translation>Por favor, aguarde. Calculando o espaço em disco a ser utilizado...</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="357"/>
        <location filename="../mainwindow.cpp" line="414"/>
        <source>Error</source>
        <translation>Ocorreu um Erro</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="358"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>O arquivo da saída %1 já existe. Por favor, utilize um outro nome para o arquivo ou exclua o arquivo existente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="363"/>
        <source>Settings</source>
        <translation>Configurações</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="369"/>
        <source>Snapshot will use the following settings:</source>
        <translation>O ‘Snapshot’ utilizará as seguintes configurações para a imagem ISO:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="371"/>
        <source>- Snapshot directory:</source>
        <translation>- Pasta das imagens ISO:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="372"/>
        <source>- Kernel to be used:</source>
        <translation>- Núcleo/kernel a ser utilizado:</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="394"/>
        <location filename="../mainwindow.cpp" line="403"/>
        <source>NVIDIA Detected</source>
        <translation>Uma placa de vídeo da NVIDIA foi detectada</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="395"/>
        <source>This computer uses an NVIDIA graphics card. Are you planning to use the resulting ISO on the same computer or another computer with an NVIDIA card?</source>
        <translation>Este computador utiliza uma placa de vídeo da NVIDIA. Você está planejando utilizar a imagem ISO resultante no mesmo computador ou em um outro computador que tem uma placa de vídeo da NVIDIA?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="399"/>
        <source>NVIDIA Selected</source>
        <translation>Uma placa de vídeo da NVIDIA foi selecionada</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="400"/>
        <source>Note: If you use the resulting ISO on a computer without an NVIDIA card, you will likely need to remove &apos;xorg=nvidia&apos; from the boot options.</source>
        <translation>Observação: Se você utilizar a imagem ISO resultante em um computador que não tem uma placa de vídeo da NVIDIA, provavelmente será necessário remover o parâmetro ‘xorg=nvidia’ das opções de inicialização.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="404"/>
        <source>Note: If you use the resulting ISO on a computer with an NVIDIA card, you may need to add &apos;xorg=nvidia&apos; to the boot options.</source>
        <translation>Observação: Se você utilizar a imagem ISO resultante em um computador que tem uma placa de vídeo da NVIDIA, pode ser necessário adicionar o parâmetro &apos;xorg=nvidia&apos; nas opções de inicialização.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="415"/>
        <source>Current kernel doesn&apos;t support selected compression algorithm, please edit the configuration file and select a different algorithm.</source>
        <translation>O núcleo (kernel) atual não é compatível com o algoritmo de compactação/compressão que foi selecionado. Por favor, edite o arquivo de configurações e escolha um algoritmo diferente.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="439"/>
        <source>Final chance</source>
        <translation>Esta é a sua última chance</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="441"/>
        <source>Snapshot now has all the information it needs to create an ISO from your running system.</source>
        <translation>O programa ‘Snapshot’ possui todas as informações necessárias para criar uma imagem ISO a partir do seu sistema operacional em execução.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="442"/>
        <source>It will take some time to finish, depending on the size of the installed system and the capacity of your computer.</source>
        <translation>O processo de criação da imagem ISO irá demorar algum tempo para finalizar. O tempo pode ser maior ou menor dependendo do tamanho do seu sistema operacional que está instalado e da capacidade de processamento do seu computador.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="444"/>
        <source>OK to start?</source>
        <translation>Você quer iniciar agora?</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="448"/>
        <source>Shutdown computer when done.</source>
        <translation>Desligar o computador quando finalizar a criação da imagem ISO.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="480"/>
        <source>Output</source>
        <translation>Resultado</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="496"/>
        <source>Close</source>
        <translation>Fechar</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="503"/>
        <source>Edit Boot Menu</source>
        <translation>Editar o Menu de Inicialização</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="504"/>
        <source>The program will now pause to allow you to edit any files in the work directory. Select Yes to edit the boot menu or select No to bypass this step and continue creating the snapshot.</source>
        <translation>O programa irá parar agora para permitir que você edite quaisquer arquivos que estão na pasta de trabalho. Clique na opção ‘Sim’ para editar o menu de inicialização ou clique na opção ‘Não’ para ignorar esta etapa e continuar com a criação da imagem ISO.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="593"/>
        <source>About %1</source>
        <translation>Sobre o %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="595"/>
        <source>Version: </source>
        <translation>Versão: </translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="596"/>
        <source>Program for creating a live-CD from the running system for MX Linux</source>
        <translation>O ‘Snapshot’ é um programa que permite criar uma imagem ISO do sistema operacional MX Linux em execução, podendo a imagem ISO ser executada em uma mídia de CD/DVD/USB e ainda possibilita a instalação em um computador.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="598"/>
        <source>Copyright (c) MX Linux</source>
        <translation>Direitos de Autor (c) MX Linux</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="600"/>
        <source>%1 License</source>
        <translation>Licença do %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="614"/>
        <source>%1 Help</source>
        <translation>Ajuda do %1</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="619"/>
        <source>Select Snapshot Directory</source>
        <translation>Selecione a Pasta para o Snapshot</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="640"/>
        <source>Confirmation</source>
        <translation>Confirmação</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="640"/>
        <source>Are you sure you want to quit the application?</source>
        <translation>Você tem certeza de que deseja encerrar o aplicativo?</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../main.cpp" line="71"/>
        <source>Tool used for creating a live-CD from the running system</source>
        <translation>Ferramenta utilizada para criar um CD/DVD executável a partir do sistema operacional em execução</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="75"/>
        <source>Use CLI only</source>
        <translation>Utilize apenas a Interface de Linha de Comando - CLI</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="79"/>
        <source>Number of CPU cores to be used.</source>
        <translation>Quantidade de núcleos do processador a serem utilizados.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="80"/>
        <source>Output directory</source>
        <translation>Pasta de saída</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="81"/>
        <source>Output filename</source>
        <translation>Nome do arquivo de saída</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="83"/>
        <source>Name a different kernel to use other than the default running kernel, use format returned by &apos;uname -r&apos;</source>
        <translation>Nomeie o núcleo (kernel) com um nome diferente do núcleo atual em execução, utilize o formato padrão exibido no resultado do comando ‘uname -r’ no emulador de terminal.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="85"/>
        <source>Or the full path: %1</source>
        <translation>Ou o caminho completo: %1</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="88"/>
        <source>Compression level options.</source>
        <translation>Opções de nível de compactação/compressão.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="89"/>
        <source>Use quotes: &quot;-Xcompression-level &lt;level&gt;&quot;, or &quot;-Xalgorithm &lt;algorithm&gt;&quot;, or &quot;-Xhc&quot;, see mksquashfs man page</source>
        <translation>Utilize as aspas: “-Xcompression-level &lt;level&gt;”, ou “-Xalgorithm &lt;algorithm&gt;”, ou “-Xhc”. Por favor, consulte a página do manual do ‘mksquashfs’</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="93"/>
        <source>Create a monthly snapshot, add &apos;Month&apos; name in the ISO name, skip used space calculation</source>
        <translation>Crie uma imagem ISO mensal com o ‘Snapshot’, adicione no nome da ISO, o nome do ‘mês’ e ignore o cálculo do espaço utilizado</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="94"/>
        <source>This option sets reset-accounts and compression to defaults, arguments changing those items will be ignored</source>
        <translation>Esta opção permite redefinir as contas e a compactação para os padrões, os argumentos que alteram estes itens serão ignorados</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="97"/>
        <source>Don&apos;t calculate checksums for resulting ISO file</source>
        <translation>Não calcular a soma de verificação para o arquivo da imagem ISO resultante</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="98"/>
        <source>Skip calculating free space to see if the resulting ISO will fit</source>
        <translation>Ignorar o cálculo do espaço livre em disco que verifica se a ISO resultante caberá ou não no disco de armazenamento</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="99"/>
        <source>Option to fix issue with calculating checksums on preempt_rt kernels</source>
        <translation>Opção para corrigir o problema com o cálculo da soma de verificação nos núcleos/kernels ‘preempt_rt’</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="100"/>
        <source>Resetting accounts (for distribution to others)</source>
        <translation>Redefinir as contas e as senhas dos usuários ‘demo’ e ‘root’ para distribuir a imagem ISO para outras pessoas</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="101"/>
        <source>Calculate checksums for resulting ISO file</source>
        <translation>Calcular a soma de verificação para o arquivo da imagem ISO resultante</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="103"/>
        <source>Throttle the I/O input rate by the given percentage. This can be used to reduce the I/O and CPU consumption of Mksquashfs.</source>
        <translation>Acelerar a taxa de entrada da E/S ou I/O pelo valor da porcentagem fornecida. Esta opção pode ser utilizada para reduzir o consumo da E/S do processador e do ‘Mksquashfs’ (ferramenta para criar e anexar sistemas de arquivos ‘squashfs’).</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="106"/>
        <source>Work directory</source>
        <translation>Pasta de trabalho</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="108"/>
        <source>Exclude main folders, valid choices: </source>
        <translation>Exclua as pastas principais, as opções válidas são:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="110"/>
        <source>Use the option one time for each item you want to exclude</source>
        <translation>Utilize a opção uma vez para cada item que você quer excluir</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="113"/>
        <source>Compression format, valid choices: </source>
        <translation>As opções válidas para o formato de compactação/compressão são:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="115"/>
        <source>Shutdown computer when done.</source>
        <translation>Desligar o computador quando finalizar a criação da imagem ISO.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="144"/>
        <source>You seem to be logged in as root, please log out and log in as normal user to use this program.</source>
        <translation>Ao que parece, você está acessando a sessão com o usuário ‘root’. Por favor, saia da sessão atual e entre novamente com o usuário normal para utilizar este programa.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="168"/>
        <source>version:</source>
        <translation>Versão:</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="163"/>
        <source>You must run this program with sudo or pkexec.</source>
        <translation>Você tem que executar este programa com o comando ‘sudo’ ou o ‘pkexec’ para ter as permissões necessárias.</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="139"/>
        <source>MX Snapshot</source>
        <translation>Snapshot do MX - Criador de Imagem ISO do Sistema Operacional</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="148"/>
        <location filename="../main.cpp" line="219"/>
        <location filename="../settings.cpp" line="240"/>
        <location filename="../settings.cpp" line="255"/>
        <location filename="../settings.cpp" line="732"/>
        <location filename="../settings.cpp" line="831"/>
        <source>Error</source>
        <translation>Ocorreu um Erro</translation>
    </message>
    <message>
        <location filename="../main.cpp" line="216"/>
        <location filename="../settings.cpp" line="249"/>
        <source>Current kernel doesn&apos;t support Squashfs, cannot continue.</source>
        <translation>O núcleo (kernel) atual não é compatível com o ‘Squashfs’. Por isso, não é possível continuar.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="85"/>
        <source>Could not create working directory. </source>
        <translation>Não foi possível criar a pasta de trabalho.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="109"/>
        <source>Could not create temp directory. </source>
        <translation>Não foi possível criar a pasta temporária.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="234"/>
        <source>Could not find a usable kernel</source>
        <translation>Não foi possível encontrar um núcleo/kernel utilizável</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="358"/>
        <source>Used space on / (root): </source>
        <translation>O espaço utilizado na / (raiz ou ‘root’) é de </translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="361"/>
        <source>estimated</source>
        <translation>foi estimado</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="370"/>
        <source>Used space on /home: </source>
        <translation>O espaço utilizado na /home é de </translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="458"/>
        <source>Free space on %1, where snapshot folder is placed: </source>
        <translation>O espaço livre na %1 onde a pasta que armazena a imagem ISO é de </translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="462"/>
        <source>The free space should be sufficient to hold the compressed data from / and /home

      If necessary, you can create more available space
      by removing previous snapshots and saved copies:
      %1 snapshots are taking up %2 of disk space.
</source>
        <translation>O espaço livre deve ser suficiente para armazenar os dados compactados da / (raiz do sistema operacional ou ‘root’) e da /home (pasta
pessoal). Se for necessário, você pode liberar mais espaço excluindo as imagens ISOs antigas que foram criadas pelo programa ‘Snapshot’.
As %1 imagens ISOs estão ocupando %2 de espaço em disco.</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="480"/>
        <source>Desktop</source>
        <translation>Área de Trabalho</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="481"/>
        <source>Documents</source>
        <translation>Documentos</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="482"/>
        <source>Downloads</source>
        <translation>Baixados</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="483"/>
        <source>Music</source>
        <translation>Músicas</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="484"/>
        <source>Networks</source>
        <translation>Redes</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="485"/>
        <source>Pictures</source>
        <translation>Imagens</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="487"/>
        <source>Videos</source>
        <translation>Vídeos</translation>
    </message>
    <message>
        <location filename="../settings.cpp" line="725"/>
        <location filename="../settings.cpp" line="824"/>
        <source>Output file %1 already exists. Please use another file name, or delete the existent file.</source>
        <translation>O arquivo da saída %1 já existe. Por favor, utilize um outro nome para o arquivo ou exclua o arquivo existente.</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="70"/>
        <source>License</source>
        <translation>Licença</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="71"/>
        <location filename="../about.cpp" line="81"/>
        <source>Changelog</source>
        <translation>Relatório de Alterações</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="72"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../about.cpp" line="94"/>
        <source>&amp;Close</source>
        <translation>&amp;Fechar</translation>
    </message>
</context>
<context>
    <name>Work</name>
    <message>
        <location filename="../work.cpp" line="82"/>
        <source>Cleaning...</source>
        <translation>Limpando...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="93"/>
        <location filename="../work.cpp" line="283"/>
        <source>Done</source>
        <translation>O processo foi realizado com sucesso</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="105"/>
        <source>Interrupted or failed to complete</source>
        <translation>O processo foi interrompido ou não foi concluído</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="138"/>
        <location filename="../work.cpp" line="241"/>
        <location filename="../work.cpp" line="263"/>
        <location filename="../work.cpp" line="301"/>
        <location filename="../work.cpp" line="414"/>
        <source>Error</source>
        <translation>Ocorreu um Erro</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="139"/>
        <source>There&apos;s not enough free space on your target disk, you need at least %1</source>
        <translation>Não há espaço livre suficiente no disco de destino, você precisa de pelo menos %1</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="142"/>
        <source>You have %1 free space on %2</source>
        <translation>Você tem %1 espaço livre em %2</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="145"/>
        <source>If you are sure you have enough free space rerun the program with -o/--override-size option</source>
        <translation>Se você tiver certeza de que tem espaço livre suficiente, execute novamente o programa com a opção ‘-o/--override-size’</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="170"/>
        <source>Copying the new-iso filesystem...</source>
        <translation>Copiar o sistema de arquivos da nova imagem ISO...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="182"/>
        <source>Could not create temp directory. </source>
        <translation>Não foi possível criar a pasta temporária.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="238"/>
        <source>Squashing filesystem...</source>
        <translation>Comprimindo o sistema de arquivos...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="242"/>
        <source>Could not create linuxfs file, please check /var/log/%1.log</source>
        <translation>Não foi possível criar o arquivo ‘linuxfs’. Por favor, verifique o registro das alterações ‘/var/log/%1.log’</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="260"/>
        <source>Creating CD/DVD image file...</source>
        <translation>Criar o arquivo de imagem de CD/DVD...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="264"/>
        <source>Could not create ISO file, please check whether you have enough space on the destination partition.</source>
        <translation>Não foi possível criar o arquivo da imagem ISO. Por favor, verifique se há espaço suficiente na partição de destino.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="270"/>
        <source>Making hybrid iso</source>
        <translation>Criar uma imagem ISO híbrida</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="288"/>
        <source>Success</source>
        <translation>Sucesso</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="289"/>
        <source>MX Snapshot completed successfully!</source>
        <translation>O Snapshot ISO do MX concluiu com sucesso a criação da imagem ISO!</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="290"/>
        <source>Snapshot took %1 to finish.</source>
        <translation>O Snapshot ISO do MX demorou %1 para terminar.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="291"/>
        <source>Thanks for using MX Snapshot, run MX Live USB Maker next!</source>
        <translation>Obrigado por utilizar o Snapshot do MX. Em seguida, execute o programa Criador de Dispositivo USB Executável do MX ou ‘MX Live USB Maker’!</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="298"/>
        <source>Installing </source>
        <translation>Instalando</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="301"/>
        <source>Could not install </source>
        <translation>Não foi possível instalar</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="311"/>
        <source>Calculating checksum...</source>
        <translation>Calculando a soma de verificação...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="349"/>
        <source>Building new initrd...</source>
        <translation>Construindo o novo ‘initrd’...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="415"/>
        <source>Could not create working directory. </source>
        <translation>Não foi possível criar a pasta de trabalho.</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="545"/>
        <source>Calculating total size of excluded files...</source>
        <translation>Calculando o tamanho total dos arquivos excluídos...</translation>
    </message>
    <message>
        <location filename="../work.cpp" line="555"/>
        <source>Calculating size of root...</source>
        <translation>Calculando o tamanho da raiz (‘root’)...</translation>
    </message>
</context>
</TS>
