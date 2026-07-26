/// Evaluates whether a published topic matches a subscriber's topic pattern.
/// Supports exact match, '+' single-level wildcard, and '#' multi-level wildcard.
///
/// Examples:
/// - pattern "sensors/+/temperature" matches "sensors/room1/temperature"
/// - pattern "sensors/#" matches "sensors/room1/temperature" and "sensors/living/humidity"
/// - pattern "a/b/c" matches only "a/b/c"
pub fn matches_topic(pattern: &str, topic: &str) -> bool {
    let pattern_parts: Vec<&str> = pattern.split('/').collect();
    let topic_parts: Vec<&str> = topic.split('/').collect();

    let mut p_idx = 0;
    let mut t_idx = 0;

    while p_idx < pattern_parts.len() {
        let p_part = pattern_parts[p_idx];

        if p_part == "#" {
            // '#' at the end matches any remaining levels (including zero levels if topic ended).
            return true;
        }

        if t_idx >= topic_parts.len() {
            return false;
        }

        let t_part = topic_parts[t_idx];

        if p_part != "+" && p_part != t_part {
            return false;
        }

        p_idx += 1;
        t_idx += 1;
    }

    t_idx == topic_parts.len()
}
