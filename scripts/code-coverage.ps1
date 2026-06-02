# calls OpenCppCoverage and produces a report in XML and HTML format
Param(
    [string]$build_dir = (Join-Path -Path (Join-Path -Path $PSScriptRoot -ChildPath ".." -Resolve) -ChildPath "build"), 
    [string]$config = "Debug",
    [string]$modules = "*_tests.exe",
    [string]$opencppcoverage_path = "OpenCppCoverage.exe",
    [string]$report_dir = (Join-Path -Path $env:temp -ChildPath "coverage_report"),
    [string]$source_dir = (Join-Path -Path (Join-Path -Path $PSScriptRoot -ChildPath ".." -Resolve) -ChildPath "src"),
    [string]$test_label = "",
    [string]$test_regex = "",
    [switch]$verbose
)

Function Save-RunnableCommand {
    Param(
        [string[]]$command_array,
        [string]$output_path
    )

    # Prepare a command string that is properly quoted for copy-pasting into PowerShell.
    $quoted_args = $command_array | ForEach-Object {
        # Quote the argument if it contains any character that isn't a "safe" shell character.
        If ($_ -match '[^a-zA-Z0-9\.\\\/_:-]') {
            "'$($_.Replace("'", "''"))'";
        } Else {
            $_;
        }
    };

    # If the executable path (first element) is quoted, PowerShell requires the call operator '&' to run it.
    If ($quoted_args[0] -match "^'") {
        $quoted_args[0] = "& $($quoted_args[0])";
    }

    # Format the command for readability: one parameter and its value per line, tab-indented.
    $formatted_lines = @($quoted_args[0]);
    $after_separator = $False;
    $is_first_after_separator = $False;

    For ($i = 1; $i -lt $quoted_args.Count; $i++) {
        $arg = $quoted_args[$i];

        # Handle the -- separator specially: it doesn't take a value and changes indentation
        If ($arg -eq '--') {
            $formatted_lines += "`t$arg";
            $after_separator = $True;
            $is_first_after_separator = $True;
            Continue;
        }

        # Determine indentation level:
        # - Arguments before --: Single-indented
        # - First argument after -- (the command): Double-indented
        # - Subsequent arguments (the command's parameters): Triple-indented
        $indent = If ($after_separator) {
            If ($is_first_after_separator) { "`t`t"; } Else { "`t`t`t"; }
        } Else {
            "`t";
        };

        # Pair flags with their values (if the next arg isn't another flag)
        If ($arg -match "^'?-+" -and ($i + 1) -lt $quoted_args.Count -and $quoted_args[$i+1] -notmatch "^'?-+") {
            $formatted_lines += "$indent$arg $($quoted_args[$i+1])";
            $i++;
        } Else {
            $formatted_lines += "$indent$arg";
        }
        $is_first_after_separator = $False;
    }

    # Join with backticks for PowerShell line continuation
    $formatted_lines -join " ``$([Environment]::NewLine)" | Out-File -FilePath $output_path -Encoding utf8;
}

# create the directory if it doesn't already exist
If (-Not (Test-Path -Path $report_dir -PathType Container)) {
    New-Item -Path $report_dir -ItemType Directory -Force | Out-Null;
}

$coverage_html = $report_dir;
$command_path = Join-Path $report_dir "coverage_command.ps1";
$coverage_exit_code_path = Join-Path $report_dir "coverage_exit_code.txt";
$coverage_log = Join-Path $report_dir "coverage.log";
$coverage_err = Join-Path $report_dir "coverage.err";
$coverage_xml = Join-Path $report_dir "coverage.xml";
$excluded_dir = Join-Path $source_dir "tests";

# build coverage command
$coverage_args = [string[]]@(
    "--cover_children",

    # the compiler creates instructions for lines that look like this:
    # } else {
    # or even just
    # }
    # see https://github.com/OpenCppCoverage/OpenCppCoverage/wiki/FAQ#else-or--is-not-marked-as-covered
    "--excluded_line_regex", "^\s*(\} )?else( \{)?\s*$",
    "--excluded_line_regex", "^\s*\}\s*$",

    "--excluded_sources", $excluded_dir,
    "--export_type", "cobertura:${coverage_xml}",
    "--export_type", "html:${coverage_html}",
    "--modules", $modules,
    "--sources", $source_dir
);

If ($verbose) {
    $coverage_args += "--verbose";
}

$ctest_args = [string[]]@(
    "--build-config", $config,
    "--test-dir", $build_dir,
    "--output-on-failure"
);

If ($test_label) {
    $ctest_args += @("-L", $test_label);
}

If ($test_regex) {
    $ctest_args += @("-R", $test_regex);
}

$coverage_args += @("--", "ctest");
$coverage_args += $ctest_args;

# write the command to the report directory
Save-RunnableCommand -command_array (@($opencppcoverage_path) + $coverage_args) -output_path $command_path;

# run code coverage
# save the log, the error log, and the exit code
& $opencppcoverage_path $coverage_args > $coverage_log 2> $coverage_err;
$exit_code = $LASTEXITCODE;
$exit_code | Out-File -FilePath $coverage_exit_code_path -Encoding utf8;

$opencppcoverage_log = "LastCoverageResults.log";
If (Test-Path $opencppcoverage_log) {
    Move-Item -Path $opencppcoverage_log -Destination $report_dir -Force;
}

Write-Host "Report at $report_dir";

# relay the exit code to the caller
Exit $exit_code;
