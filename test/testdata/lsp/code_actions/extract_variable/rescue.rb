# typed: true
# selective-apply-code-action: refactor.extract
# enable-experimental-lsp-extract-to-variable: true

begin
  1 + 1
#     ^ apply-code-action: [A] Extract Variable
rescue Errno::ENOENT
  2 + 2
#     ^ apply-code-action: [B] Extract Variable
rescue ArgumentError
  3 + 3
#     ^ apply-code-action: [C] Extract Variable
else
  4 + 4
#     ^ apply-code-action: [D] Extract Variable
ensure
  5 + 5
#     ^ apply-code-action: [E] Extract Variable
end

begin
ensure
  6 + 6
#     ^ apply-code-action: [F] Extract Variable
end
