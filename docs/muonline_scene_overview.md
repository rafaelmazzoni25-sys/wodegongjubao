# MuOnline World Editing Flow

Este documento resume como o editor de mundo incluso neste repositório carrega, exibe e permite editar cenários do MuOnline.

## Carregamento de plugins e seleção de formato

* A janela principal (`CDlgMainEditor`) inicializa um gerenciador de plugins de dados e carrega todos os descritores `.dsc` da pasta `Plugins`. Isso habilita suportes de importação/exportação específicos para cada formato de mapa.【F:WorldEditor/Dialog/DlgMainEditor.cpp†L151-L264】
* Ao abrir um arquivo, o diálogo escolhe o plugin com a extensão apropriada e chama `importData` passando a cena renderizável (`getDisplay().getScene()`).【F:WorldEditor/Dialog/DlgMainEditor.cpp†L254-L264】

## Plugin do MuOnline (`SoulScenePlug`)

O plugin `CMyPlug` implementa a ponte entre os arquivos do MuOnline e as interfaces genéricas do editor.

* Arquivos `.map` são lidos via `CLumpFile`, extraindo largura, altura, tamanho dos cubos e o vetor de células para preencher `iTerrainData`.【F:SoulScenePlug/MyPlug.cpp†L75-L95】
* Os recursos de objetos são obtidos de `object.csv`, associando IDs a nomes e caminhos de modelos resolvidos a partir da pasta do mapa.【F:SoulScenePlug/MyPlug.cpp†L98-L118】
* O arquivo `.obj` binário é carregado e descriptografado com uma rotina XOR/adição antes de criar cada instância de objeto 3D com posição, rotação e escala convertidas para as unidades internas do editor.【F:SoulScenePlug/MyPlug.cpp†L23-L67】【F:SoulScenePlug/MyPlug.cpp†L130-L160】
* As configurações ambientais (neblina e luz direcional) são recuperadas do `.sce`; em seguida o plugin cria o terreno, carrega materiais de tiles (`Tile.csv`), monta a árvore espacial de objetos e importa a malha de objetos.【F:SoulScenePlug/MyPlug.cpp†L162-L188】

## Renderização do cenário

* O componente visual `CUIWorldEditorDisplay` mantém câmera, cena (`CScene`) e uma instância de `CTerrainEditor`. A cada frame ele atualiza parâmetros de shader, aplica as matrizes de visão/projeção e chama `m_Scene.UpdateRender` seguido de `m_Scene.render` para desenhar o terreno e objetos carregados.【F:WorldEditor/Dialog/UIWorldEditorDisplay.cpp†L49-L139】
* O método `MoveCamera` desloca o ponto alvo da câmera conforme entradas WASD ou arrasto com botão do meio, sempre mantendo a altura colada ao terreno para facilitar a navegação.【F:WorldEditor/Dialog/UIWorldEditorDisplay.cpp†L388-L417】

## Ferramentas de edição

* Quando o usuário pressiona o botão esquerdo, o editor marca o início de uma operação (para suportar desfazer/refazer) e, dependendo do modo, seleciona/move objetos ou ativa o pincel de terreno.【F:WorldEditor/Dialog/UIWorldEditorDisplay.cpp†L399-L470】【F:WorldEditor/Dialog/UIWorldEditorDisplay.cpp†L500-L520】
* Durante o arrasto com o pincel ativo, `m_Terrain.Brush` é chamado continuamente com a posição sob o cursor. A força do pincel pode ser invertida com SHIFT, e o alvo de objetos utiliza snapping à grade e ao chão do terreno.【F:WorldEditor/Dialog/UIWorldEditorDisplay.cpp†L399-L470】
* `CTerrainEditor::Brush` roteia a ação do pincel para funções específicas: altura, atributos (flags), cores de vértice e tiles das camadas 1/2. Valores negativos limpam tiles, positivos aplicam o ID selecionado.【F:WorldEditor/TerrainEditor.cpp†L688-L727】
* Cada função de pincel popula um `MAP_EDIT_RECORD` com os valores afetados, chamando `doEdit` para aplicar as mudanças e registrar os valores antigos. Isso garante atualização da malha (IB/VB) e dá suporte a undo/redo através das pilhas `m_setReback` e `m_setRedo`.【F:WorldEditor/TerrainEditor.cpp†L123-L340】
* O método `Render` de `CTerrainEditor` controla a visualização das camadas, atributos e grade, além de desenhar o decal do pincel, de modo que o artista veja exatamente o que está modificando.【F:WorldEditor/TerrainEditor.cpp†L68-L110】

## Resumo do fluxo

1. `CDlgMainEditor` carrega plugins, abre o pacote MuOnline escolhido e delega ao `CMyPlug` a importação dos arquivos `.sce`, `.map`, `.obj` e `.csv`.
2. O plugin traduz os dados proprietários em interfaces genéricas (`iScene`/`iTerrainData`), inclusive descriptografando os objetos.
3. `CUIWorldEditorDisplay` usa essas interfaces para atualizar a cena e renderizá-la em tempo real, enquanto `CTerrainEditor` fornece pincéis e controle de visualização.
4. Interações de mouse/teclado chamam `Brush` ou manipulam objetos, com histórico para undo/redo.

Com esses componentes, o editor consegue carregar cenários do MuOnline, exibir o mundo completo e oferecer ferramentas de edição interativas.
