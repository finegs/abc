/// 🔹 3. STRATEGY PATTERN
/// Encapsulates pagination calculation algorithms into interchangeable strategy objects.
pub trait PaginationStrategy: Send + Sync {
    fn calculate_limit_and_offset(
        &self,
        page: Option<usize>,
        limit: Option<usize>,
    ) -> (i32, i32);
}

/// Default offset/limit strategy
#[derive(Debug, Clone)]
pub struct DefaultPaginationStrategy {
    pub default_limit: usize,
    pub max_limit: usize,
}

impl DefaultPaginationStrategy {
    pub fn new(default_limit: usize, max_limit: usize) -> Self {
        Self {
            default_limit,
            max_limit,
        }
    }
}

impl Default for DefaultPaginationStrategy {
    fn default() -> Self {
        Self::new(10, 100)
    }
}

impl PaginationStrategy for DefaultPaginationStrategy {
    fn calculate_limit_and_offset(
        &self,
        page: Option<usize>,
        limit: Option<usize>,
    ) -> (i32, i32) {
        let page_num = page.unwrap_or(1).max(1);
        let raw_limit = limit.unwrap_or(self.default_limit);
        let effective_limit = raw_limit.clamp(1, self.max_limit);

        let offset = (page_num - 1) * effective_limit;
        (effective_limit as i32, offset as i32)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_default_pagination_strategy() {
        let strategy = DefaultPaginationStrategy::default();

        // Default page=1, limit=10
        let (limit, offset) = strategy.calculate_limit_and_offset(None, None);
        assert_eq!(limit, 10);
        assert_eq!(offset, 0);

        // Page 3, limit 20
        let (limit, offset) = strategy.calculate_limit_and_offset(Some(3), Some(20));
        assert_eq!(limit, 20);
        assert_eq!(offset, 40);

        // Max limit cap check (e.g., limit 500 capped to 100)
        let (limit, _) = strategy.calculate_limit_and_offset(Some(1), Some(500));
        assert_eq!(limit, 100);
    }
}
