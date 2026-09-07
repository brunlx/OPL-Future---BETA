# OPL FUTURE — Test Plan (QA Phase 2)

Objetivo: validar o frontend OPL FUTURE (tema FUTURE com elementos novos: Carousel, SelectorBar, StatusBar e Frame) no OPNPS2LD (Open PS2 Loader) refatorado.

Situação do ambiente:
- Sem acesso físico a um PlayStation 2. Nenhum teste deste plano pôde ser executado em hardware real.
- Todos os itens abaixo contam com análise estática de código e validação de build/ELF, mas **NÃO** foram executados num PS2.
- O operador de teste deve executar cada checklist e registrar PASS / FAIL / N/A, com evidência.

Artefatos sob teste:
- `OPNPS2LD.ELF` (empacotado) — ELF MIPS 32-bit LSB, linkado estaticamente.
- `opl.elf` (não empacotado, com símbolos) e `opl_stripped.elf`.
- Tema `misc/conf_theme_OPL.cfg` (theme FUTURE), embutido no binário via `conf_theme_OPL.o`.

---

## 1. BOOT

| ID | Verificação | Método |
|----|-------------|--------|
| B1 | OPNPS2LD.ELF inicia sem travar (sem tela preta permanente) | Hardware — boot via memcard/USB/HDD |
| B2 | Tela de carregamento aparece e Não fica presa em loop | Hardware |
| B3 | Nenhum crash/reboot inesperado nos primeiros 30s | Hardware |
| B4 | Sem mensagens de erro de inicialização de rede/HDD não tratadas que impeçam o menu | Hardware |
| B5 | Logs (se disponíveis via serial/OPL log) sem falhas de alocação no tema | Hardware/serial |

## 2. NAVIGATION

| ID | Verificação |
|----|-------------|
| N1 | Navegação entre dispositivos (BDM/ETH/HDD/APP) alterna corretamente SelectorBar e StatusBar |
| N2 | Seleção de item atualiza o Carousel (cover central + vizinhos) |
| N3 | Paginação com muitos jogos (>14 itens) sem estouro/sobreposição |
| N4 | Alternância entre aba Jogos e Aplicativos usa listas distintas (appsMain ItemsList) |
| N5 | Sem travamento ao navegar em listas com 0, 1 ou muitos itens |
| N6 | Linha dupla de navegação (Tab main + submenu) sem conflito de foco |

## 3. GAME BROWSER

| ID | Verificação |
|----|-------------|
| G1 | Cover do jogo selecionado renderiza no centro do Carousel |
| G2 | Covers vizinhos renderizam menores aos lados, na direção correta |
| G3 | Jogos sem arte exibem arte padrão (default) em vez de glitch |
| G4 | Mais de `neighbors` de distância não desenham covers indevidos |
| G5 | Carregamento de arte é assíncrono (não bloqueia a UI) |
| G6 | Informações do jogo (info0-18) com painel Frame e GameImage à direita |

## 4. DEVICES

| ID | Verificação |
|----|-------------|
| D1 | StatusBar mostra estado real (ONLINE/OFFLINE/READY/STANDBY) por dispositivo |
| D2 | SelectorBar mostra dot verde/acinzentado conforme device habilitado |
| D3 | Ícone do dispositivo não distorce quando textura de largura atípica/0 |
| D4 | Mudança de modo de start (manual/auto/off) reflete no `visible`/estado |
| D5 | Setup com 5+ modos BDM + ETH + HDD + APP habilitados não causa OOB na StatusBar |

## 5. UI / THEMING

| ID | Verificação |
|----|-------------|
| U1 | Tema FUTURE carrega com todos os elementos (Background/Frame/ItemsList/SelectorBar/StatusBar/Carousel) |
| U2 | Texto do Carousel/Slot alinhado e sem corte |
| U3 | Cores do tema aplicadas (selTextColor/uiTextColor/default) |
| U4 | Backward-compat: tema antigo (sem tipos novos) carrega sem erro |
| U5 | Escala/posição correta em 640×480 (coords hardcoded do tema) |
| U6 | Alterar tema em runtime não deixa cache órfão nem crash (submenuRebuildCache) |

## 6. VIDEO

| ID | Verificação |
|----|-------------|
| V1 | Sem "flicker"/artefatos ao rolar o Carousel |
| V2 | Gradientes (rmDrawRectVGrad) renderizam sem listras quando altura pequena |
| V3 | Sem tearing perceptível em 60Hz |
| V4 | Após mudança de resolução/modo, tema não distorce |

## 7. STABILITY

| ID | Verificação |
|----|-------------|
| S1 | 30 min de navegação contínua sem crash ou vazamento visível |
| S2 | Troca repetida Jogos↔Aplicativos sem degradação |
| S3 | Entrar/sair repetidamente da tela Info sem crash |
| S4 | Tema com patch de artwork (ICON/COV/SCR) não cresce memória indefinidamente (LRU cap 8) |
| S5 | Reprodução de jogo após navegação longa funciona (se device disponível) |

---

## Resultado (a preencher pelo operador)

| Fase | Compilou | Arquitetura | ELF empacotado | Hardware |
|------|----------|-------------|----------------|----------|
| Build (2026-09-06) | SIM (GCC 15.2 mips64r5900el) | MIPS 32-bit LSB | SIM (58.47%) | N/A — sem PS2 |

Legenda de status a usar no relatório final:
- STATIC ANALYSIS
- BUILD TEST
- ELF VALIDATION
- HARDWARE TEST (somente com evidência real em PS2)
