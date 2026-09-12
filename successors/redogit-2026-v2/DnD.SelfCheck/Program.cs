using DnD.Core;

var checks = new[]
{
    CheckParseAndRange(),
    CheckModifier(),
    CheckDeterministicRoll(),
    CheckInvalidExpression()
};

foreach (var check in checks)
    Console.WriteLine($"{check.Name}: {(check.Passed ? "PASS" : "FAIL")}");

return checks.All(check => check.Passed) ? 0 : 1;

static Check CheckParseAndRange()
{
    var parsed = DiceExpression.Parse("2d6 + 3");
    return new("dice parse and range", parsed == new DiceExpression(2, 6, 3) && parsed.Minimum == 5 && parsed.Maximum == 15);
}

static Check CheckModifier() =>
    new("ability modifier", Rules.AbilityModifier(8) == -1 && Rules.AbilityModifier(10) == 0 && Rules.AbilityModifier(18) == 4);

static Check CheckDeterministicRoll()
{
    var total = DiceExpression.Parse("2d6+3").Roll(new SequenceRandomSource(4, 5));
    return new("deterministic roll", total == 12);
}

static Check CheckInvalidExpression()
{
    try
    {
        _ = DiceExpression.Parse("not dice");
        return new("invalid expression rejected", false);
    }
    catch (FormatException)
    {
        return new("invalid expression rejected", true);
    }
}

sealed record Check(string Name, bool Passed);
