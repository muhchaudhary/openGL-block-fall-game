{
  description = "OpenGL block fall game";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    graphify-src = {
      url = "github:safishamsi/graphify";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, graphify-src }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};

    graphify = pkgs.python3Packages.buildPythonApplication {
      pname = "graphify";
      version = "0.8.23";
      pyproject = true;

      src = graphify-src;

      build-system = [ pkgs.python3Packages.setuptools ];

      nativeBuildInputs = [ pkgs.python3Packages.pythonRelaxDepsHook ];

      # These tree-sitter language packages are not in nixpkgs; their imports
      # are inside try/except blocks in graphify so missing ones are skipped.
      pythonRemoveDeps = [
        "tree-sitter-typescript"
        "tree-sitter-go"
        "tree-sitter-java"
        "tree-sitter-groovy"
        "tree-sitter-c"
        "tree-sitter-cpp"
        "tree-sitter-ruby"
        "tree-sitter-kotlin"
        "tree-sitter-scala"
        "tree-sitter-php"
        "tree-sitter-swift"
        "tree-sitter-lua"
        "tree-sitter-zig"
        "tree-sitter-powershell"
        "tree-sitter-elixir"
        "tree-sitter-objc"
        "tree-sitter-julia"
        "tree-sitter-verilog"
        "tree-sitter-fortran"
        "tree-sitter-dm"
      ];

      propagatedBuildInputs = with pkgs.python3Packages; [
        networkx
        datasketch
        rapidfuzz
        tree-sitter
        tree-sitter-python
        tree-sitter-javascript
        tree-sitter-json
        tree-sitter-bash
        tree-sitter-rust
        tree-sitter-c-sharp
        tree-sitter-sql
      ];

      doCheck = false;
    };
  in {
    devShells.${system}.default = pkgs.mkShell {
      nativeBuildInputs = with pkgs; [ gcc gnumake ];
      buildInputs = with pkgs; [
        freeglut
        libGL
        libGLU
        graphify
      ];
    };
  };
}
