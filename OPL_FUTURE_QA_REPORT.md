# OPL FUTURE — QA REPORT

Relatório das Fases 2 (QA estático) e 3 (correção de bugs identificados em teste de hardware) do frontend OPL FUTURE do OPNPS2LD refatorado.
Data: 2026-09-06
Ambiente de análise: apenas estático + build/ELF. **Nenhum teste em PS2 físico foi realizado.**

---

## 1. STATUS GERAL

Fase 2 concluída e Fase 3 (feedback de hardware) aplicada. Itens da Fase 2:

- Auditoria estática de memória do código novo e das áreas tocadas (themes, textcache, menusys, renderman).
- 2 correções defensivas reais (BUG-001/002).
- Correção do alvo de empacotamento do Makefile (BUG-003, bônus de build).
- Build limpo (objetos concernidos recompilados) com GCC 15.2 mips64r5900el.
- ELF validado: arquitetura MIPS 32-bit, linkado estático, empacotado com sucesso.
- Plano de teste criado (OPL_FUTURE_TEST_PLAN.md).

Fase 3 (feedback de hardware `Jogos no USB` sem jogos + tela principal FUTURE):

- Carousel vazio/“frame vazio dentro” → **empty-state dedicado** (BUG-101) + fallback para capa default do tema no cover central (BUG-102).
- Texto duplicado “Jogos no USB” ×3 (título, status bar, dock inferior) → **StatusBar compacta** (BUG-103) e **SelectorBar sem nome repetido** (BUG-104).
- Pequenos elementos ciano/brancos e “barra inferior desalinhada” → tiles do Dock normatizados (ícone + dot status) (BUG-104).
- Novo build validado: `OPNPS2LD.ELF` 1 347 044 B.

Status final: **PASS (build + análise estática) / NÃO VERIFICADO EM HARDWARE**.

Sem acesso a PS2 → nenhuma garantia de comportamento em hardware real é emitida aqui.

---

## 2. BUGS ENCONTRADOS

### BUG-001 — SelectorBar: divisão por zero em `iconTex->Width`
- Arquivo/Linha: `src/themes.c:1135`
- Causa: `int iconH = (iconTex->Height * iconW) / iconTex->Width;` executado quando `iconTex` é válido (`Mem`) mas `Width == 0` (textura corrompida/degenerada em tema de terceiros).
- Impacto: crash (exceção de divisão por zero no MIPS) se uma textura de ícone tiver largura 0. Improvável mas possível com pacotes de tema malformados.
- Correção: adicionar guarda `iconTex->Width > 0` na condição.
- Risco: baixo (muda apenas o caso degenerado, onde antes crashava).
- Teste: análise estática + build. Em hardware, só se manifestaria com textura inválida.

### BUG-002 — StatusBar: índice de rótulos podia exceder array local `labels[8][64]`
- Arquivo/Linha: `src/themes.c:1209`
- Causa: o primeiro passe preenche `labels[i]` apenas enquanto `count < 8`; o segundo passe desenhava para **todas** as entradas visíveis incrementando `i` sem limite. Com mais de 8 dispositivos visíveis (`it->item->visible`), `i` passaria de 7 → leitura fora do array local (UB / corrupção de pilha). Hoje o máximo de dispositivos no menu é 8 (MODE_COUNT), então o cenário não é atingível no código atual, mas a defesa faltava.
- Impacto: leitura OOB na pilha se o número de dispositivos visíveis exceder 8 (ex.: futura expansão de modos, ou tema/backport que injete itens extras).
- Correção: restringir o segundo passe com `&& i < count`.
- Risco: nulo para o comportamento atual (mesmo layout, pois `count <= 8`).
- Teste: análise estática + build. Em hardware, verificar com 5+ modos BDM + ETH + HDD + APP habilitados (checklist D5).

### BUG-003 — Makefile: alvo de empacotamento quebrava com `cd`
- Arquivo/Linha: `Makefile:387`
- Causa: `(cd $(PS2_PACKER_DIR) && ./ps2-packer $< $@)` — após `cd`, `$<` e `$@` relativos não resolviam (erro "Unable to open input file").
- Impacto: `make OPNPS2LD.ELF` falhava; era preciso invocar ps2-packer manualmente com caminhos absolutos.
- Correção: usar `$(CURDIR)/$<` e `$(CURDIR)/$@`.
- Risco: nenhum.
- Teste: `make OPNPS2LD.ELF` OK (ver seção 5).

### BUG-101 (Fase 3) — Carousel vazio / “frame vazio por dentro” sem jogos
- Arquivo/Linha: `src/themes.c` — `drawCarousel` (linha ~985) e `drawCarouselEmptyState` (nova, linha ~950).
- Causa raiz (observado em hardware: USB `Jogos no USB` sem itens): `drawCarousel` retornava cedo em `if (!item)` (e `item == NULL` quando não há submenu), deixando apenas o Frame (main4) desenhado → área central enorme vazia. Não existia nenhum estado de lista vazia no frontend.
- Correção: novo estado vazio dedicado centrado no carousel — nome do dispositivo (localizado, via `menuItemGetText`), mensagem `_l(_STR_NO_ITEMS)`, linhas decorativas e dica de atualização (`SELECT_ICON` + `_l(_STR_REFRESH)`). Usa apenas primitivas de desenho existentes; zero alocações no caminho de frame.
- Teste: estático + build. Em hardware: entrar em `Jogos no USB` com pen drive vazio → empty-state central deve aparecer.

### BUG-102 (Fase 3) — Cover central do carousel ficava vazio enquanto a arte carregava / sem arte
- Arquivo/Linha: `src/themes.c` — `drawCarousel`, bloco “main cover” (linha ~1048).
- Causa raiz: o cover central desenhava somente `getGameImageTexture(...)`; quando a arte não havia sido carregada (async) ou o jogo não tinha capa, nada era renderizado na posição central — visual de “carousel vazio” mesmo com jogos na lista, enquanto os covers vizinhos usavam fallback `carousel->image.defaultTexture`.
- Correção: replicar o fallback dos vizinhos no cover central — `if (!texture || !texture->Mem) texture = &carousel->image.defaultTexture->source;`.
- Risco: baixo (mesma política já aplicada aos vizinhos; mantém desenhar nada apenas se também não houver capa default — caso em que agora o cover central desenha o frame/glow sem pixmap se a textura default ainda estiver ausente, nunca crash).

### BUG-103 (Fase 3) — Texto duplicado “Jogos no USB” no topo (título + StatusBar)
- Arquivo/Linha: `src/themes.c` — `drawStatusBar` (linha ~1159).
- Causa raiz: a StatusBar (main3, y=42) impressa `menuItemGetText(it->item)` = “Jogos no USB” + status “READY” + separador para **cada** dispositivo visível → aparecia redundante logo abaixo do título (main2, y=16) e gerava a linha “Jogos no USB READY -”.
- Correção: desenhar apenas o status compacto do dispositivo atual (`READY`/`STANDBY`/`ONLINE`/`OFFLINE`, com bullet de acento colorido) — o nome do dispositivo já vive no título.
- Risco: layout do topo muda visualmente (removida duplicação intencional); compatível com temas antigos (elemento opcional).

### BUG-104 (Fase 3) — Barra inferior: nome repetido + “elementos ciano/brancos em posições estranhas”
- Arquivo/Linha: `src/themes.c` — `drawSelectorBar` (linha ~1080).
- Causa raiz: cada tile do dock (main8, centred y≈450) desenhava o nome completo do dispositivo abaixo do ícone → “Jogos no USB” aparecia sobre a região da barra inferior; somado aos dots de status e ícones sem alinhamento, a dock parecia desalinhada com elementos soltos (ciano/branco).
- Correção: tiles normatizados — pill transparente (gradiente + frame para o selecionado), dot de status no canto e ícone do dispositivo centralizado; o nome repetido foi removido (o título identifica o dispositivo). Slot mínimo (40px) e largura máxima da tile (`width>>1`) para evitar tiles gigantes quando há poucos dispositivos; linha centralizada quando é menor que o elemento.
- Teste: estático + build. Em hardware: com só USB ativa, a dock deve mostrar um único tile com ícone + dot verde centralizado, sem texto sobreposto.

---

## 3. BUGS CORRIGIDOS

- **BUG-001** — guarda `iconTex->Width > 0` em `drawSelectorBar` (`src/themes.c:1133`).
- **BUG-002** — guarda `i < count` no segundo passe de `drawStatusBar` (`src/themes.c:1209`).
- **BUG-003** — alvo `$(EE_BIN_PACKED)` do Makefile com caminhos absolutos.
- **BUG-101** — empty-state dedicado no carousel quando não há itens (novo `drawCarouselEmptyState`).
- **BUG-102** — fallback `defaultTexture` no cover central do carousel.
- **BUG-103** — `drawStatusBar` compacta: só o status do dispositivo atual, sem echo do nome.
- **BUG-104** — `drawSelectorBar` com tiles ícone + dot (nome removido), dimensões saneadas e centralização.

---

## 4. ARQUIVOS MODIFICADOS

### Frontend OPL FUTURE (salvo em fase anterior, revalidado aqui):
| Arquivo | Alteração |
|---|---|
| `src/themes.c` | Elementos novos Carousel/SelectorBar/StatusBar/Frame + helpers; BUG-001/002; **Fase 3: empty-state (BUG-101), fallback default cover central (BUG-102), StatusBar compacta (BUG-103), tiles do dock normatizados (BUG-104)** |
| `include/themes.h` | `carousel_t`, `frame_t`, novos `ELEM_TYPE_*` |
| `src/renderman.c` / `include/renderman.h` | `rmDrawRectOutline`, `rmDrawFrame`, `rmDrawRectVGrad`, `rmUnScaleX` |
| `misc/conf_theme_OPL.cfg` | Tema FUTURE (main/apps/info), embutido no binário — **sem mudanças na Fase 3** |
| `asm/conf_theme_OPL.c` | Gerado por bin2s a partir do cfg |

### QA / BUILD:
| Arquivo | Alteração |
|---|---|
| `Makefile` | Correção do alvo packer (BUG-003) |
| `OPL_FUTURE_TEST_PLAN.md` | Criado (plano de teste) |
| `OPL_FUTURE_UI.md` | Documentação do frontend (fase anterior) |
| `AI_PROGRESS.md` | Log de progresso (fase anterior, desatualizado re: data) |
| `OPL_FUTURE_QA_REPORT.md` | Este relatório (Fases 2 + 3) |

Arquivos binários regenerados: `obj/conf_theme_OPL.o`, `obj/themes.o`, `opl.elf`, `opl_stripped.elf`, `OPNPS2LD.ELF`, e artefatos de módulos (reconstruídos como efeito do `make`).

---

## 5. BUILD

Comandos executados (Fases 2 e 3):

```
make all NOT_PACKED=1 PS2_PACKER_DIR=/usr/local/ps2dev/bin   # ELF não empacotado
make all PS2_PACKER_DIR=/usr/local/ps2dev/bin                # strip+pack via Makefile
```

- Toolchain: `/usr/local/ps2dev` — `mips64r5900el-ps2-elf-gcc (GCC) 15.2.0`, padrão C composto GNU17 ok, `-Wall`, sem `-Werror`.
- Fase 3: `rm -f obj/themes.o && make obj/themes.o …` → **sem warnings novos** (único aviso: `LOG` redefinida em `ioman.h`, pré-existente).
- `obj/themes.o` recompilado; `obj/conf_theme_OPL.o` da Fase 2 (cfg não mudou).

---

## 6. ELF

| Artefato | Tamanho | Timestamp | Validação |
|---|---|---|---|
| `opl.elf` | 10 336 184 B | 2026-09-06 21:46 | ELF 32-bit LSB, MIPS N32 MIPS-III, linkado estático, com símbolos |
| `opl_stripped.elf` | 3 242 676 B | 2026-09-06 21:46 | ELF 32-bit LSB, MIPS N32 MIPS-III, stripped |
| `OPNPS2LD.ELF` | 1 347 044 B | 2026-09-06 21:46 | ELF 32-bit LSB, MIPS-I, linkado estático, packer via Makefile OK |

`file OPNPS2LD.ELF`: `ELF 32-bit LSB executable, MIPS, MIPS-I version 1 (SYSV), statically linked, no section header`. Empacotamento agora funciona pelo próprio `make` (BUG-003 corrigido). SHA-256 de `OPNPS2LD.ELF`: `591317b12fd6f7769eef6a7841d885eca2c55f05bee2e3e01b881b8c9ec1ed52`.

---

## 7. PERFORMANCE

- **Carousel**: desenha somente `neighbors` (default 1) de cada lado + cover central; de 0 a `neighbors` só percorre a lista linear (parada em NULL). Sem laço infinito, mesmo com lista longa.
- **Cache de arte**: cada texto/cover pedido vai a `cacheGetTexture`; carregamento é assíncrono (ioRequestLoadTextAsync), com LRU global limitado a 8 caches e `TEXCACHE_MAX_ENTRIES` por cache. `submenuRebuildCache` realoca os arrays de `cache_id`/`cache_uid` conforme `gameCacheCount`, e é re-disparado na troca de tema (opl.c:260 e 1239) — sem crescimento infinito.
- **Por frame**: nenhuma alocação heap nova no caminho de desenho dos novos elementos (buffers em stack, structs pré-alocados no init). Quads extras por frame são poucos (gradiente/outline), sem impacto relevante.
- **StatusBar/SelectorBar**: após a Fase 3, a StatusBar desenha só o status do dispositivo atual (O(1), sem laço de dispositivos); a SelectorBar mantém um passe por dispositivo visível (≤ MODE_COUNT=8) — O(n) trivial, sem alocações.

---

## 8. COMPATIBILIDADE

- **Backward-compat de temas**: temas antigos (sem os tipos novos) não criam os elementos novos; os `drawElem`/`endElem` default continuam usados. As mudanças em `drawItemsList` (gradiente/acento) aplicam-se a qualquer tema que use ItemsList — mudança visual intencional, sem quebra funcional.
- **Coordenadas**: tema FUTURE usa lógica 640×480 (screenWidth/screenHeight), com regras `x<0 → screenWidth+x`, `y<0 → ceil((screenHeight+y)*usedHeight/screenHeight)`, tokens `POS_MID`/`DIM_INF`, `aligned=1` = ALIGN_CENTER.
- **App mode**: `appsMain5` ItemCover com overlay `apps_case` (cantos 0/9/168/19/0/170/168/180) para visual clássico; `appsMain7` ItemsList garante paginação em apps.
- **findDuplicate** só reutiliza caches de tipos STATIC/ATTRIBUTE/GAME/BACKGROUND; o Carousel nunca é fonte de deduplicação → `appsMain5` têm cache próprio (aceitável, caches ≤ 8 LRU).
- **Localização**: empty-state usa `_l(_STR_NO_ITEMS)` (localizado) e `menuItemGetText` (nome do dispositivo localizado); StatusBar mantém palavras curtas fixas EN (areia do console OPL — limitado, não bug).

---

## 9. TESTES NÃO REALIZADOS POR FALTA DE PS2

Sem acesso físico ao console, os itens a seguir **não foram executados** — são cobertos apenas por análise estática:

- Boot real (OPNPS2LD.ELF em memcard/USB/HDD).
- Navegação com gamepad real (inputs CROSS/CIRCLE/DS4).
- Carregamento de arte real (itemGetImage/with startup) via rede/HDD/USB.
- Comportamento de vídeo (>640×480, 60Hz, tearing) em tela real.
- Empacotado em memcard/pasta SMB real.
- Testes de estresse de longa duração (30min+).
- Qualquer assert de "funciona no PS2".

Conclusão honesta: o frontend está **analisado estaticamente e compilado/validado como ELF**, mas **não testado em hardware**.

---

## 10. PROCEDIMENTO EXATO PARA TESTE NO HARDWARE

1. Copiar `OPNPS2LD.ELF` para a mídia de boot do PS2 (memcard: `BOOT/BOOT.ELF`; ou via SMB/HDD de acordo com o método usado).
2. Bootar; aguardar tela principal do OPL com tema FUTURE.
3. Executar o checklist de `OPL_FUTURE_TEST_PLAN.md` na ordem: BOOT (B1–B5), NAVIGATION (N1–N6), GAME BROWSER (G1–G6), DEVICES (D1–D5), UI/THEMING (U1–U6), VIDEO (V1–V4), STABILITY (S1–S5).
4. Registrar PASS/FAIL/N/A por item, com foto/vídeo como evidência.
5. Caso algum elemento não apareça: revisar `misc/conf_theme_OPL.cfg` (nomes/ordem dos blocos) e conferir os logs via serial.
6. Em falha de arte (glitch/zero): validar as guardas BUG-001/002 (dimensão de textura/ícone e nº de dispositivos) com um tema de teste com texturas degeneradas e 5+ modos BDM.
7. **Fase 3 — validação específica das correções visuais:** (a) bootar com pen drive sem jogos e confirmar o empty-state central no carousel (BUG-101); (b) com 1 jogo **sem capa** e **com capa**, confirmar cover central preenchido (fallback default / arte) (BUG-102); (c) confirmar que “Jogos no USB” aparece apenas no título superior e o status compacto `READY` logo abaixo, sem “Jogos no USB READY -” (BUG-103); (d) conferir a dock inferior: tiles ícone + dot, sem texto “Jogos no USB” sobreposto (BUG-104).
8. Atualizar este relatório com o status HARDWARE TEST e mudar a conclusão da seção 9 quando houver evidência.

---

## Anexo — resumo de garantias

| Camada | Status |
|---|---|
| Análise estática (memória, laços, limites) | PASS |
| Build limpo (GCC 15.2, sem warnings novos) | PASS |
| Validação de ELF (arquitetura/link/empacote) | PASS |
| Teste em PS2 físico | NÃO REALIZADO |