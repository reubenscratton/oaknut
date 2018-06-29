# encoding: utf-8

Gem::Specification.new do |s|
  s.name          = "jekyll-theme-oaknut"
  s.version       = "0.1.0"
  s.license       = "CC0-1.0"
  s.authors       = ["Reuben Scratton", "Sandcastle Software"]
  s.email         = ["reuben.scratton@gmail.com"]
  s.homepage      = "https://oaknut.io"
  s.summary       = "Jekyll theme for Oaknut"

  s.files         = `git ls-files -z`.split("\x0").select do |f|
    f.match(%r{^((_includes|_layouts|_sass|assets)/|(LICENSE|README)((\.(txt|md|markdown)|$)))}i)
  end

  s.platform      = Gem::Platform::RUBY
  s.add_runtime_dependency "jekyll", "~> 3.5"
end
