# Contributing to FSP

Thank you for considering contributing to the Forest Simulation Platform! This document provides guidelines for contributing to the project.

## 🌟 Ways to Contribute

- **Bug Reports**: Submit detailed bug reports via GitHub Issues
- **Feature Requests**: Propose new features or improvements
- **Code Contributions**: Submit pull requests for bug fixes or new features
- **Documentation**: Improve README, guides, or code documentation
- **Testing**: Help test new features and report issues
- **Scientific Validation**: Validate simulation results with field data

## 🚀 Getting Started

### 1. Set Up Development Environment

```bash
# Fork and clone the repository
git clone https://github.com/litteshoes/FSP.git
cd FSP

# Install Git LFS
git lfs install
git lfs pull

# Build Docker images
docker compose build --build-arg INSTALL_BLENDER=1

# Start development environment
docker compose up -d
```

### 2. Development Workflow

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**:
   - Write clean, documented code
   - Follow the project's code style
   - Add tests if applicable

3. **Test your changes**:
   ```bash
   # Run the application
   docker compose up -d
   
   # Check logs
   docker compose logs -f
   
   # Run tests (if available)
   pytest tests/
   ```

4. **Commit your changes**:
   ```bash
   git add .
   git commit -m "feat: add amazing new feature"
   ```
   
   Use conventional commit format:
   - `feat:` for new features
   - `fix:` for bug fixes
   - `docs:` for documentation changes
   - `refactor:` for code refactoring
   - `test:` for adding tests
   - `chore:` for maintenance tasks

5. **Push and create Pull Request**:
   ```bash
   git push origin feature/your-feature-name
   ```
   
   Then open a Pull Request on GitHub.

## 📋 Contribution Areas

### 🐛 Bug Fixes

Found a bug? Great! Please:

1. **Check existing issues** to avoid duplicates
2. **Create a new issue** with:
   - Clear description of the bug
   - Steps to reproduce
   - Expected vs actual behavior
   - Environment details (OS, Docker version, etc.)
   - Error logs if applicable
3. **Submit a pull request** with the fix

### ✨ New Features

Want to add a feature? Please:

1. **Open an issue first** to discuss the feature
2. **Wait for approval** before starting implementation
3. **Follow the project architecture**
4. **Add documentation** for the new feature
5. **Include examples** if applicable

### 📚 Documentation

Documentation improvements are always welcome:

- Fix typos or unclear explanations
- Add examples and use cases
- Improve installation instructions
- Translate documentation (especially to Chinese)
- Add inline code comments

### 🧪 Testing

Help improve test coverage:

- Write unit tests for existing functions
- Add integration tests for workflows
- Test on different platforms (Linux, macOS, Windows)
- Validate simulation results against field data

## 💻 Code Style Guidelines

### Python Code Style

Follow [PEP 8](https://pep8.org/) with these specifics:

- **Indentation**: 4 spaces (no tabs)
- **Line length**: Maximum 100 characters (prefer 80)
- **Imports**: Group in order: standard library, third-party, local
- **Naming conventions**:
  - Functions and variables: `snake_case`
  - Classes: `PascalCase`
  - Constants: `UPPER_SNAKE_CASE`
  - Private methods: `_leading_underscore`

### Code Documentation

- **Docstrings**: Use Google-style docstrings
- **Comments**: Explain "why", not "what"
- **Type hints**: Use type hints where appropriate

Example:

```python
def calculate_biomass(trees: list[dict], allometry: str = "default") -> float:
    """
    Calculate total biomass from tree inventory.
    
    Args:
        trees: List of tree dictionaries with 'dbh' and 'height' keys
        allometry: Allometric equation to use ('default', 'tropical', 'temperate')
        
    Returns:
        Total biomass in kg/ha
        
    Raises:
        ValueError: If allometry type is not supported
        
    Example:
        >>> trees = [{'dbh': 45.2, 'height': 23.5}, {'dbh': 38.7, 'height': 21.2}]
        >>> calculate_biomass(trees, 'tropical')
        12543.7
    """
    # Implementation here
    pass
```

### JavaScript Code Style

- Use modern ES6+ syntax
- Use `const` and `let`, avoid `var`
- Use template literals for string interpolation
- Add JSDoc comments for functions

### HTML/CSS

- Semantic HTML5 tags
- Consistent indentation (2 spaces)
- Use CSS classes over inline styles
- Mobile-responsive design

## 🧪 Testing Guidelines

### Writing Tests

```python
import pytest
from mymodule import my_function

def test_my_function_success():
    """Test my_function with valid input."""
    result = my_function(valid_input)
    assert result == expected_output
    
def test_my_function_error():
    """Test my_function with invalid input."""
    with pytest.raises(ValueError):
        my_function(invalid_input)
```

### Running Tests

```bash
# Run all tests
pytest

# Run specific test file
pytest tests/test_simulation.py

# Run with coverage
pytest --cov=app tests/
```

## 📝 Pull Request Process

### Before Submitting

- [ ] Code follows style guidelines
- [ ] Tests pass locally
- [ ] Documentation updated
- [ ] Commit messages follow convention
- [ ] No merge conflicts with main branch

### PR Description Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Refactoring
- [ ] Performance improvement

## Testing
Describe how you tested your changes

## Screenshots (if applicable)
Add screenshots for UI changes

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings generated
- [ ] Tests added/updated
```

### Review Process

1. **Automated checks**: CI/CD will run tests
2. **Code review**: Maintainer will review your code
3. **Requested changes**: Address any feedback
4. **Approval**: Once approved, PR will be merged

## 🔬 Scientific Contributions

### Model Validation

Help validate FORMIND simulations:

- Compare results with field data
- Test parameter sensitivity
- Document assumptions and limitations
- Suggest calibration improvements

### Climate Data

- Add new climate scenario support
- Improve climate data preprocessing
- Validate climate projections
- Document data sources

### Visualization

- Improve 3D rendering quality
- Add new visualization types (charts, maps)
- Optimize Blender export performance
- Create visualization templates

## 📄 Licensing

By contributing, you agree that your contributions will be licensed under the same **AGPL v3.0+ license** as the project.

This means:
- Your code will be open source
- Modifications must be shared
- Network use counts as distribution

See [LICENSE](LICENSE) for full details.

## 🆘 Getting Help

- **GitHub Discussions**: Ask questions at [GitHub Discussions](https://github.com/litteshoes/FSP/discussions)
- **Issues**: Report bugs at [GitHub Issues](https://github.com/litteshoes/FSP/issues)
- **Documentation**: Check existing docs in `/docs` directory
- **Email**: Contact maintainers (see README)

## 🌍 Community Guidelines

### Code of Conduct

- Be respectful and inclusive
- Welcome newcomers
- Accept constructive criticism
- Focus on what's best for the community
- Show empathy towards others

### Communication

- Use clear, concise language
- Provide context for decisions
- Be patient with questions
- Acknowledge contributions
- Celebrate successes

## 🎯 Priority Areas

Current development priorities:

1. **Performance optimization**: Improve simulation speed
2. **UI/UX improvements**: Make interface more intuitive
3. **API documentation**: Complete API reference
4. **Mobile support**: Responsive design for tablets/phones
5. **Internationalization**: Multi-language support
6. **Advanced analytics**: More statistical analysis tools

## 📊 Development Roadmap

See [GitHub Projects](https://github.com/litteshoes/FSP/projects) for detailed roadmap.

## 🙏 Recognition

All contributors will be recognized in:
- GitHub contributors page
- Project README
- Release notes
- (For significant contributions) CITATION.cff

Thank you for helping make FSP better! 🌳

