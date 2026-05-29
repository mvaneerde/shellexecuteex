# calls OpenCppCoverage and produces a report in XML and HTML format
Param(
    [string]$build_dir = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".." "build")),
    [string]$config = "Debug",
    [string]$opencppcoverage_path = "OpenCppCoverage.exe",
    [string]$report_dir = (Join-Path $env:temp "coverage_report"),
    [string]$source_dir = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".." "src")),
    [switch]$verbose
)

$coverage_html = $report_dir
$coverage_xml = Join-Path $report_dir "coverage.xml"
$excluded_dir = Join-Path $source_dir "tests"

If ($verbose) {
    $verbose_arg = @("--verbose")
} Else {
    $verbose_arg = @()
}

& OpenCppCoverage.exe `
    --cover_children `
    --excluded_sources $excluded_dir `
    --export_type cobertura:${coverage_xml} `
    --export_type html:${coverage_html} `
    --modules *.exe `
    --sources $source_dir `
    $verbose_arg `
    -- `
    ctest `
        --build-config $config `
        --test-dir $build_dir `
        --output-on-failure

$return_code = $LASTEXITCODE

$opencppcoverage_log = "LastCoverageResults.log"
If (Test-Path $opencppcoverage_log) {
    Move-Item -Path $opencppcoverage_log -Destination $report_dir -Force
}

Exit $return_code
