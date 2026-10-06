# ServeExcel predecessor boundary

Historical source is retained in-place and is not ported into this runtime.

- repository: `BanalityOfSeeking/ServeExcel`
- historical `FunctionalObject.cs`: ordered Dataflow buffers/transforms/checks
- historical `Pipelines.cs`: HTTP listener routing / URL command dispatch
- historical `ExcelTemplateManager.cs`: URL-encoded command parsing plus EPPlus export
- historical `TemplateObject.cs`: report/sheet/format/content object shape

The current `redogit/DnD` lineage already records the 2019 `FunctionalObject.cs` source anchor and the later 2021 .NET syntax update in `projects/decision-field/docs/FUNCTIONALOBJECT_ANYFUNCTOR_LINEAGE_2026-10-03.md`.

This subtree is a bounded successor for spreadsheet generation only:

`data -> inferred schema -> fluent builder -> self-described binary input -> C/Wasm XLSX emitter -> file bytes`

It deliberately does not copy the listener, responder hierarchy, URL parser, mutable report registry, Dataflow pipeline, EPPlus dependency, or C# runtime.

`HISTORICAL_SHAPE != CURRENT_RUNTIME`
`SUCCESSOR != REWRITTEN_PREDECESSOR`
`SOURCE_IDENTITY != RENDERING`
