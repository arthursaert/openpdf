# OpenPDF

OpenPDF é um leitor de código aberto feito para as pessoas lerem PDFs de forma simplificada, ele possuí as funcionalidades de:

- Selecionar textos
- Destacar e criar anotações

## Compilação
O OpenPDF já é pré-compilado por padrão para Linux, mas se quiser compilar, siga esses passos:

### 1
Clonar repositório Git na pasta que você quiser
```bash
mkdir openpdf && cd openpdf
```
```bash
git clone https://github.com/arthursaert/openpdf.git
```
### 2
Compilar OpenPDF
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```
```bash
make -j$(nproc)
```

> O OpenPDF é feito especialmente para Linux, pode ser preciso modificações para compilar para o Windows ou Mac.

**Haverá futuras atualizações para esse leitor**, espero que goste!

> O OpenMusic está disponível sobre a licença Apache 2.0.
