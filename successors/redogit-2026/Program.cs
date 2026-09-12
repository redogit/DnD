using System.Text.RegularExpressions;

namespace DnD.Redo;

public readonly partial record struct DiceExpression(int Count, int Sides, int Modifier)
{
    [GeneratedRegex(@"^\s*(?<count>\d*)d(?<sides>\d+)(?<modifier>\s*[+-]\s*\d+)?\s*$", RegexOptions.IgnoreCase)]
    private static partial Regex DicePattern();

    public static DiceExpression Parse(string text)
    {
        var match = DicePattern().Match(text ?? string.Empty);
        if (!match.Success) throw new FormatException($"Invalid dice expression: '{text}'.");

        var count = string.IsNullOrWhiteSpace(match.Groups["count"].Value)
            ? 1
            : int.Parse(match.Groups["count"].Value);
        var sides = int.Parse(match.Groups["sides"].Value);
        var modifierText = match.Groups["modifier"].Value.Replace(" ", string.Empty);
        var modifier = string.IsNullOrEmpty(modifierText) ? 0 : int.Parse(modifierText);

        if (count <= 0) throw new FormatException("Dice count must be positive.");
        if (sides <= 1) throw new FormatException("Die must have at least two sides.");

        return new(count, sides, modifier);
    }

    public int Roll(IRandomSource random)
    {
        var total = Modifier;
        for (var i = 0; i < Count; i++)
            total += random.Next(1, Sides + 1);
        return total;
    }
}

public interface IRandomSource
{
    int Next(int minInclusive, int maxExclusive);
}

public sealed class SystemRandomSource(int? seed = null) : IRandomSource
{
    private readonly Random _random = seed is null ? Random.Shared : new Random(seed.Value);
    public int Next(int minInclusive, int maxExclusive) => _random.Next(minInclusive, maxExclusive);
}

public sealed class SequenceRandomSource(params int[] values) : IRandomSource
{
    private readonly Queue<int> _values = new(values);

    public int Next(int minInclusive, int maxExclusive)
    {
        if (_values.Count == 0) throw new InvalidOperationException("No deterministic values remain.");
        var value = _values.Dequeue();
        if (value < minInclusive || value >= maxExclusive)
            throw new InvalidOperationException($"Deterministic value {value} is outside [{minInclusive}, {maxExclusive}).");
        return value;
    }
}

public static class Rules
{
    public static int AbilityModifier(int score) => (int)Math.Floor((score - 10) / 2.0);
}

public static class Program
{
    public static int Main()
    {
        var checks = new[]
        {
            CheckParse(),
            CheckModifier(),
            CheckDeterministicRoll()
        };

        foreach (var check in checks)
            Console.WriteLine($"{check.Name}: {(check.Passed ? "PASS" : "FAIL")}");

        return checks.All(c => c.Passed) ? 0 : 1;
    }

    private static Check CheckParse()
    {
        var parsed = DiceExpression.Parse("2d6 + 3");
        return new("dice parsing", parsed == new DiceExpression(2, 6, 3));
    }

    private static Check CheckModifier() =>
        new("ability modifier", Rules.AbilityModifier(8) == -1 && Rules.AbilityModifier(10) == 0 && Rules.AbilityModifier(18) == 4);

    private static Check CheckDeterministicRoll()
    {
        var parsed = DiceExpression.Parse("2d6+3");
        var total = parsed.Roll(new SequenceRandomSource(4, 5));
        return new("deterministic roll", total == 12);
    }

    private sealed record Check(string Name, bool Passed);
}
