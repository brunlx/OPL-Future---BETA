# OPL FUTURE — Open PS2 Loader (BETA)

> **O futuro do Open PS2 Loader.** Um fork moderno do OPL com frontend redesenhado,
> tema neon "FUTURE", carousel de capas, dock de dispositivos e barra de status HUD.

[![CI](https://github.com/brunlx/OPL-Future---BETA/actions/workflows/compilation.yml/badge.svg)](https://github.com/brunlx/OPL-Future---BETA/actions/workflows/compilation.yml)
[![CI-format](https://github.com/brunlx/OPL-Future---BETA/actions/workflows/check-format.yml/badge.svg)](https://github.com/brunlx/OPL-Future---BETA/actions/workflows/check-format.yml)
[![Latest Release](https://img.shields.io/github/v/release/brunlx/OPL-Future---BETA?label=Release)](https://github.com/brunlx/OPL-Future---BETA/releases)
[![Downloads](https://img.shields.io/github/downloads/brunlx/OPL-Future---BETA/total?color=green)](https://github.com/brunlx/OPL-Future---BETA/releases)
[![License](https://img.shields.io/badge/License-AFL%203.0-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-PS2%20%2F%20PS3%20BC-black)]()

---

## ✨ O que torna o OPL FUTURE especial

O **OPL FUTURE** não é apenas um build do OPL — é uma **reinvenção do frontend**.
A interface foi completamente redesenhada com o tema **FUTURE**: um estilo
dark de alta contraste, inspirado em HUD de consoles modernos.

![FUTURE theme layout](https://raw.githubusercontent.com/brunlx/OPL-Future---BETA/main/misc/preview.png)

### 🎨 O tema FUTURE (100% data-driven)

Tudo é configurado por um único arquivo — `misc/conf_theme_OPL.cfg` — sem novas
APIs: novos tipos de elemento foram adicionados ao motor de temas do OPL.

| Elemento | Descrição |
| --- | --- |
| **Carousel** | Carousel de capas horizontal, com capa central ampliada, brilho em gradiente e moldura neon nas laterais |
| **SelectorBar** | Dock inferior de dispositivos (HDD · Ethernet · USB · MX4SIO · APPS) com pills, ícones e dots de status |
| **StatusBar** | Barra HUD compacta no topo, mostrando o status de cada dispositivo (READY / STANDBY / ONLINE / OFFLINE) |
| **Frame** | Moldura decorativa neon: 4 cantoneiras + bordas + tint translúcido interno |
| **ItemsList** | Lista de jogos renovada: barra de seleção em gradiente + acento ciano neon |

```
┌────────────────────────────────────────────────────────┐
│ [icon]  < DEVICE NAME >                    [### GAMES] │
│            STATUS BAR (device statuses)                │
│  ┌────────────────────────┐        ┌───────────────┐   │
│  │      GAMES LIST        │        │  NEON FRAME   │   │
│  │  ┌──────────────────┐  │        │   [COVER]     │   │
│  │  │▶ selected game   │  │        │   [COVER]     │   │
│  │  └──────────────────┘  │        └───────────────┘   │
│  │                        │        « CANVAS / ITEM »    │
│  ════════════ BDM ════════    !                        │
│  HDD ▪ Ethernet ▪ USB ▪ MX4SIO ▪ APPS  (SELECTOR BAR) │
└────────────────────────────────────────────────────────┘
```

Coordenação em espaço lógico **640×480**, independente de resolução, com
cores de alto contraste selecionadas para leitura em CRT e vídeo-componente.
Veja [`OPL_FUTURE_UI.md`](OPL_FUTURE_UI.md) para a documentação completa do design.

### 🛡️ Correções de qualidade (Fase 2 + Fase 3)

Além do tema, este fork corrigiu bugs reais encontrados na auditoria de código:

- **SFX silencioso — bug crítico P0:** variável de laço compartilhada impedia o
  carregamento de *todos* os efeitos sonoros. Corrigido.
- **Divisão por zero** no `SelectorBar` com texturas degeneradas (BUG-001).
- **Leitura fora dos limites** na `StatusBar` com muitos dispositivos (BUG-002).
- **Empacotamento do ELF via `make`** — o alvo `ps2-packer` do Makefile quebrava
  com `cd` relativo; corrigido com caminhos absolutos (BUG-003).
- **Carousel vazio** sem jogos → novo *empty-state* dedicado (BUG-101).
- **Capa padrão** no cover central quando a arte não carrega (BUG-102).
- **StatusBar compacta** — sem nome de dispositivo duplicado (BUG-103).
- **SelectorBar** — tiles normalizados: ícone + dot, sem texto sobreposto (BUG-104).

<details>
  <summary><b>Qualidade & estado do projeto</b></summary>
  <p>

   | Camada | Status |
   |---|---|
   | Análise estática (memória, laços, limites) | ✅ PASS |
   | Build limpo (GCC 15.2 MIPS, sem warnings novos) | ✅ PASS |
   | Validação de ELF (arquitetura / link / empacotamento) | ✅ PASS |
   | Teste em PS2 físico | ⏳ A aguardar hardware |

   Relatórios detalhados: [`OPL_FUTURE_QA_REPORT.md`](OPL_FUTURE_QA_REPORT.md),
   [`OPL_FUTURE_TEST_PLAN.md`](OPL_FUTURE_TEST_PLAN.md),
   [`AI_PROGRESS.md`](AI_PROGRESS.md).

  </p>
</details>

---

## 🚀 Recursos

Open PS2 Loader (OPL) é o loader de jogos e aplicações **100% open source** para
PlayStation 2 e PS3 (Backward Compatible) — o mais compatível na cena homebrew.

### Dispositivos suportados

| # | Dispositivo |
| --- | --- |
| 1 | USB Mass Storage |
| 2 | MX4SIO (cartão SD no slot de memory card via adaptador) |
| 3 | iLink (dispositivos SBP2 via IEEE 1394) |
| 4 | SMBv1 (pastas compartilhadas) |
| 5 | HDD interno ATA/IDE (APA/PFS) e HDD em exFAT/MBR/GPT |

### Formatos de arquivo

- ISO
- ZSO (ISO comprimido)
- USB Extreme (`.ul`)
- Homebrew / Apps em ELF
- HDDs suportam o formato HDLoader

### Outros destaques

- Cheats no formato PS2RD (`.cht`)
- Modos **GSM**, **IGS**, **PADEMU**, **VMC** e controle parental
- Servidor **NBD** embutido (compartilhe o HDD pela rede com `lwNBD`)
- 28 idiomas (gerados de fontes YAML)
- Temas 100% configuráveis (novos `ELEM_TYPE_CAROUSEL / SELECTOR_BAR / STATUS_BAR / FRAME`)
- Suporte a fragmentação parcial (até 64 fragmentos)

---

## 📂 Estrutura de pastas (HDD / SMB / USB)

| Pasta | Descrição | Modos |
| --- | --- | --- |
| `CD` | Jogos em mídia CD | USB e SMB |
| `DVD` | Imagens DVD5/DVD9 (FS precisa suportar +4GB) | USB e SMB |
| `VMC` | Imagens de Memory Card virtual — 8MB a 64MB | todos |
| `CFG` | Configurações por jogo | todos |
| `ART` | Capas / artes dos jogos | todos |
| `THM` | Temas | todos |
| `LNG` | Traduções | todos |
| `CHT` | Arquivos de cheats | todos |
| `APPS` | ELFs (apps/homebrew) | todos |

> O OPL cria essa estrutura automaticamente no primeiro boot com o dispositivo ativo.

---

## ⚡ Como usar

1. Baixe o ELF mais recente na aba **[Releases](https://github.com/brunlx/OPL-Future---BETA/releases)** (`OPNPS2LD.ELF`).
2. Copie para o seu método de boot favorito:
   - Memory Card: `BOOT/BOOT.ELF` (via uLaunchELF);
   - USB: pasta `APPS/` ou `mass:APPS/OPNPS2LD.ELF`;
   - HDD: partição `+OPL`.
3. Organize seu dispositivo conforme a tabela acima (`DVD/`, `CD/`, etc.).
4. No primeiro boot, aguarde a criação automática das pastas.

### Adicionar apps (exemplo)

```ini
# conf_apps.cfg (método legado)
OPL=mass:APPS/OPNPS2LD.ELF
```

```ini
# método title.cfg (na pasta do app, dentro de APPS/)
title=My App Name
boot=MYAPP.ELF
```

> ⚠️ O OPL é **case-sensitive** — respeite maiúsculas/minúsculas nos caminhos.

### Cheats

Arquivos `.cht` no formato PS2RD, um por jogo, devem ficar em `CHT/`.
Dois modos: **Auto Select Cheats** (aplica todos) ou **Select Game Cheats**
(menu de seleção no boot; *Mastercodes* não podem ser desabilitados).

### Servidor NBD (HDD na rede)

Compartilhe o HDD interno do PS2 como um disco de rede:

```sh
# Linux
sudo modprobe nbd
nbd-client -l 192.168.1.45          # listar export
nbd-client 192.168.1.45 /dev/nbd1   # conectar
```

```sh
# Linux (nbdfuse)
mkdir ps2 && nbdfuse ps2/ nbd://192.168.1.45 &
```

```powershell
# Windows (wnbd)
wnbd-client.exe map hdd0 192.168.1.22
```

Use `hdl-dump`, `pfs-shell` ou um editor hexadecimal normalmente sobre o disco.

### ZSO (ISO comprimido)

```sh
python pc/ziso.py -c 2 input.iso output.zso    # comprimir
python pc/ziso.py -c 0 input.zso output.iso    # descomprimir
```

---

## 🧩 Tipos de release

| Tipo | Descrição |
| --- | --- |
| `Release` | OPL padrão com GSM, IGS, PADEMU, VMC, PS2RD Cheat Engine e controle parental |
| `DTL_T10000` | OPL para TOOLs (PS2 DevKit) |
| `IGS` | OPL com screenshot em jogo |
| `PADEMU` | OPL com emulação de pad para DS3 & DS4 |
| `RTL` | OPL com suporte a idiomas da direita para a esquerda |

---

## 🗣️ Idiomas

28 idiomas são gerados a partir das fontes YAML em `lng_src/` usando o
[`lang_compiler.py`](lang_compiler.py). Compilados vão para `lng/lang_*.lng`
— o OPL gerencia o restante pela pasta `LNG/`.

---

## 🛠️ Compilando do zero

Pré-requisitos: o [**PS2SDK atual**](https://github.com/ps2dev/ps2sdk) (toolchain
MIPS `mips64r5900el-ps2-elf`) e o pacote `ps2-packer`.

```sh
# Full build (compila frontend, EE core, módulos IOP, idiomas e empacota)
make all PS2_PACKER_DIR=/usr/local/ps2dev/bin

# Apenas o ELF (sem empacotamento)
make all NOT_PACKED=1 PS2_PACKER_DIR=/usr/local/ps2dev/bin

# Release (variantes + changelog)
make release

# Formatação / verificação
make format
make format-check
```

Artefatos de build **não são rastreados no git** — são regenerados pelo `make`:

| Artefato | Descrição |
| --- | --- |
| `opl.elf` | ELF EE sem empacotar (~10 MB) |
| `opl_stripped.elf` | ELF sem símbolos (~3.2 MB) |
| `OPNPS2LD.ELF` | ELF final empacotado, pronto para o PS2 (~1.3 MB) |

---

## 🔧 Solução de problemas

<details>
  <summary><b>OPL congela no logo ou tela cinza</b></summary>
  <p>Segure <code>START</code> durante a inicialização para pular o carregamento de
  configuração de builds antigos, depois salve suas configurações novamente.</p>
</details>

<details>
  <summary><b>Jogo congela na tela branca</b></summary>
  <p>O executável principal não foi encontrado — o jogo está fragmentado ou a
  imagem está corrompida.</p>
</details>

<details>
  <summary><b>OPL não exibe nada no boot</b></summary>
  <p>O modo de vídeo pode não ser suportado pela sua TV. Segure <code>Triangle + Cross</code>
  durante a inicialização para resetar o modo de vídeo para <code>Auto</code>.</p>
</details>

---

## 📚 Documentação interna

| Arquivo | Conteúdo |
| --- | --- |
| [`OPL_FUTURE_UI.md`](OPL_FUTURE_UI.md) | Design do tema FUTURE (elementos, layouts, coordenadas) |
| [`OPL_FUTURE_QA_REPORT.md`](OPL_FUTURE_QA_REPORT.md) | Relatório de QA — Fases 2 e 3, bugs 001–104 |
| [`OPL_FUTURE_TEST_PLAN.md`](OPL_FUTURE_TEST_PLAN.md) | Checklist de teste em hardware (BOOT, NAV, GAMES, DEV, UI, VIDEO, STAB) |
| [`AI_PROGRESS.md`](AI_PROGRESS.md) | Log de progresso do projeto e arquitetura |
| [`CHANGELOG`](CHANGELOG) | Histórico de versões |

---

## 🙏 Créditos

Copyright 2013 **Ifcaro** & **jimmikaelkael** — licenciado sob
**Academic Free License v3.0**.

Baseado no OPL original ([ps2homebrew/Open-PS2-Loader](https://github.com/ps2homebrew/Open-PS2-Loader)),
que por sua vez é baseado no **HD Project 1.081** de **Polo**, com a contribuição
de muitos desenvolvedores da cena PS2 homebrew. Veja [`CREDITS`](CREDITS).

---

## ⚖️ Licença

Este projeto é licenciado sob a **Academic Free License versão 3.0**.
Consulte o arquivo [`LICENSE`](LICENSE) para mais detalhes.