#
# runner: Runner for Reia's test suite
# Copyright (C)2008-10 Tony Arcieri
# 
# Redistribution is permitted under the MIT license.  See LICENSE for details.
#

started_at = erl.now()
Main.load("test/test_helper.re")

tests = [
  ("builtins", [
    "binary", "boolean", "list", "tuple", "string", 
    "range", "regexp", "numeric", "dict", "atom", "fun"
  ]),
  ("core", [
    "blocks", "branching", "funrefs", "object", 
    "operator", "exception", "messaging"
  ]),
  ("lib",  ["json"])
]

results = tests.map do |(group, modules)|
  modules.map do |name|
    try
      Main.load("test/#{group}/#{name}_test.re")

      mod = "#{name.capitalize()}Test".to_module()
      mod.run()
    catch ex
      Main.print("E")
      (:error, "#{group}/#{name}.re", ex)
    end
  end
end.flatten()

Main.puts("\n")

failures = [error for error = (:error, _, _, _, _) in results]
failures.each do |(:error, group, description, expected, actual)| 
  Main.puts("'#{group} #{description}' FAILED")
  Main.puts("expected #{expected.inspect()}, actual #{actual.inspect()}\n")
end

errors = [error for error = (:error, _, _) in results]
errors.each do |(:error, test, ex)|
  case ex
  when (:exception,(:throw, msg))
    Main.puts("#{test} ERROR: #{msg.to_s()}")
  when _
    Main.puts("#{test} ERROR: #{ex}\n")
  end
end

finished_at = erl.now()
duration = TestHelper.duration(started_at, finished_at)

Main.puts("Finished in #{duration} seconds\n")
Main.puts("#{results.size()} assertions, #{failures.size()} failures, #{errors.size()} errors")
System.halt(1) if failures.size() > 0 or errors.size() > 0