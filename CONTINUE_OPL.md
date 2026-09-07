# OPL FUTURE — Continuar O Projeto

Instruções para continuar o projeto **OPL FUTURE (OPL Refatorado BETA)** a partir
do estado atual do repositório.

## Estado Atual (2026-09-06)

- **Fase concluída**: Fases 1–3 (auditoria, correções críticas, redesign do
  frontend FUTURE e correções visuais de hardware-feedback).
- **Build**: ✅ Funcional. Toolchain completa em `/usr/local/ps2dev`
  (`mips64r5900el-ps2-elf-gcc 15.2.0`, `ps2-packer`, SDK IOP/EE).
- **ELFs gerados**: `opl.elf` (~10,3 MB), `opl_stripped.elf` (~3,2 MB),
  `OPNPS2LD.ELF` (~1,3 MB, empacotado via `make` — BUG-003 corrigido).
- **Teste em PS2 físico**: ⏳ Ainda não realizado (ver `OPL_FUTURE_TEST_PLAN.md`).

## Comandos de Build

```bash
# Build completo (frontend + EE core + módulos IOP + idiomas + empacotamento)
make all PS2_PACKER_DIR=/usr/local/ps2dev/bin

# Sem empacotamento (gera apenas opl.elf)
make all NOT_PACKED=1 PS2_PACKER_DIR=/usr/local/ps2dev/bin

# Variantes / debug
make release
make debug
make clean && make all
```

> O alvo de empacotamento usa caminhos absolutos (`$(CURDIR)/$<` e `$(CURDIR)/$@`),
> corrigindo o bug original do `cd $(PS2_PACKER_DIR)` (BUG-003).

## Validação

```bash
file OPNPS2LD.ELF        # ELF 32-bit LSB executable, MIPS, static, no section header
sha256sum OPNPS2LD.ELF
make oplversion          # exibe a versão embutida no ELF
make format-check        # verificação de formatação (requer clang-format)
```

## Próximos Passos

1. **Teste de hardware** — seguir `OPL_FUTURE_TEST_PLAN.md` (BOOT, NAV, GAMES,
   DEV, UI, VIDEO, STAB) em um PS2 real e registrar PASS/FAIL por item.
2. **Validação visual FUTURE** — checklist da seção 7 de
   `OPL_FUTURE_QA_REPORT.md` (BUG-101 a 104).
3. **Auditoria dos arquivos restantes** — ~25 fontes já auditadas
   estaticamente; revisar qualquer alteração nova.
4. **Atualização de releases** — publicar novos ELFs na aba Releases.

## Recuperação / Ponto de Restauração

- Estado do git: branch `main`, sincronizado com `origin` do repositório
  `brunlx/OPL-Future---BETA`.
- Documentos de referência: `AI_PROGRESS.md`, `OPL_FUTURE_QA_REPORT.md`,
  `OPL_FUTURE_TEST_PLAN.md`, `OPL_FUTURE_UI.md`.
- Backup da pré-limpeza do repositório: espelho em `/tmp/opencode/OPL-backup.git`
  (não comitar).

## Notas

- Artefatos de build **não são rastreados no git** (ver `.gitignore`); um clone
  limpo regenera tudo via `make`.
- `download_lng.sh` / `download_lwNBD.sh` / `download_cfla.sh` buscam fontes de
  idiomas, lwNBD e clang-format quando ausentes (rede necessária só na 1ª
  compilação).
- Alterações "Do Not Revert": PADEMU `?= 0`, correção do laço de som em
  `sound.c`, remoção da declaração redundante de `configGetStat` em `opl.c`,
  `-O2` fora do caminho não-debug.