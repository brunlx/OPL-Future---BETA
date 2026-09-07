# OPL FUTURE — AI Checkpoint

## Fase Atual

**Fases 1–3 concluídas**: auditoria profunda (Fase 2), correções críticas,
redesign do frontend **FUTURE** e correções visuais de hardware-feedback
(Fase 3). Build validado. Teste em PS2 físico pendente.

**Última atualização**: 2026-09-06

## Estado Resumido

| Item | Status |
| --- | --- |
| Projeto | Open PS2 Loader (OPL) refatorado — fork **OPL FUTURE - BETA** |
| Git | Branch `main`, limpo, `origin = github.com/brunlx/OPL-Future---BETA` |
| Toolchain | `/usr/local/ps2dev` — `mips64r5900el-ps2-elf-gcc 15.2.0` + `ps2-packer` |
| Build | ✅ `make all` compila, reduz e empacota `OPNPS2LD.ELF` |
| Análise estática | ✅ PASS (memória, laços, limites) |
| Teste em hardware | ⏳ Não realizado (sem PS2 físico) |

## Correções Aplicadas

### Fase 2 (auditoria)
1. **sound.c (P0)** — variável de laço `int i` de escopo global compartilhada
   entre `sfxInitDefaults()` e `sfxInit()` impedia o carregamento de todos os
   efeitos sonoros. Corrigido com laço local.
2. **Makefile (PADEMU)** — `PADEMU ?= 1 → ?= 0` (menos módulos IOP carregados).
3. **Makefile (`-O2`)** — removido do caminho não-debug (conflito de variáveis).
4. **opl.c** — declaração redundante de `configGetStat` removida.

### Fase 3 (FUTURE + hardware-feedback)
5. **THEME FUTURE** — novos elementos em `src/themes.c`: `ELEM_TYPE_CAROUSEL`,
   `ELEM_TYPE_SELECTOR_BAR`, `ELEM_TYPE_STATUS_BAR`, `ELEM_TYPE_FRAME`;
   constantes no `misc/conf_theme_OPL.cfg`; novo layout (main/apps/info).
6. **BUG-001** — guarda `iconTex->Width > 0` no SelectorBar (divisão por zero).
7. **BUG-002** — guarda `i < count` no segundo passe da StatusBar (OOB).
8. **BUG-003** — alvo de empacotamento do Makefile com `$(CURDIR)` absoluto.
9. **BUG-101** — empty-state dedicado no carousel sem jogos.
10. **BUG-102** — fallback da capa `default` no cover central.
11. **BUG-103** — StatusBar compacta (só o status do dispositivo atual).
12. **BUG-104** — SelectorBar com tiles ícone + dot, sem nome repetido.

## Arquivos-Chave Modificados

- `src/themes.c`, `include/themes.h` — novos elementos/estruturas do tema
- `src/renderman.c`, `include/renderman.h` — novos primitivos de desenho
- `misc/conf_theme_OPL.cfg` — tema FUTURE embutido no binário
- `src/sound.c`, `src/opl.c`, `Makefile` — correções de qualidade

## Artefatos

| Artefato | Tamanho |
| --- | --- |
| `opl.elf` | ~10 336 184 B |
| `opl_stripped.elf` | ~3 242 676 B |
| `OPNPS2LD.ELF` | ~1 347 044 B (empacotado, sem headers de seção) |

## Riscos Atuais

- **Médio**: nenhum teste em PS2 físico — comportamento de vídeo/entrada não
  garantido (ver `OPL_FUTURE_QA_REPORT.md` §9).
- **Baixo**: artefatos de build não são rastreados (por design).

## Referências

- `OPL_FUTURE_QA_REPORT.md` — relatório QA (Fases 2+3)
- `OPL_FUTURE_TEST_PLAN.md` — checklist de testes em hardware
- `OPL_FUTURE_UI.md` — documentação de design do tema FUTURE
- `AI_PROGRESS.md` — log contínuo de progresso