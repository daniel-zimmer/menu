# menu

Simple Linux Application Launcher

---

## How It Works

1. Finds all applications
2. Caches the results
3. Provides a **menu** to choose the application (using [dmenu](https://tools.suckless.org/dmenu/))
4. Executes the application.

## How to Install

Make sure you have `git`, `make`, `gcc`, `mlocate` and `dmenu` installed.

On a debian based distro, you can run:

```sh
sudo apt install git make gcc dmenu mlocate
```

Then, you just have to clone the repository and run `make build install` inside it:

```sh
git clone https://github.com/daniel-zimmer/menu.git
cd menu
make build install
```

## How It Really Works

1. If the cache doesn't exist, uses `mlocate` to find all *.desktop* files inside `/applications/` directories and caches the result in `~/.cache/menu/menu-cache.txt`.
2. Reads all the names and executables from the cache into a hashmap.
3. Pipes all the hashmap keys (names) to dmenu.
4. Reads the name of the chosen application and uses the hashmap to get the executable.
5. Execvp's the executable.
