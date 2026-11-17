// Mainly here to catch a missing `await`: `m.sleep(500)` without it returns
// at once and the game races ahead.
import tseslint from 'typescript-eslint';

export default tseslint.config(
    { ignores: ['dist/', 'scripts/', 'test/', 'eslint.config.mjs'] },
    {
        files: ['src/**/*.ts'],
        extends: [tseslint.configs.base],
        languageOptions: {
            parserOptions: { projectService: true },
        },
        rules: {
            '@typescript-eslint/no-floating-promises': 'error',
            '@typescript-eslint/no-misused-promises': 'error',
            '@typescript-eslint/await-thenable': 'error',
        },
    },
);
